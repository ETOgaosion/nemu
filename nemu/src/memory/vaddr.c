/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
    return paddr_read(NULL, addr, len);
}

word_t vaddr_read(Decode *s, vaddr_t addr, int len) {
    if (!isa_mmu_check(addr, len, MEM_TYPE_READ)) {
        return paddr_read(s, addr, len);
    } else {
#ifdef CONFIG_MTRACE
        if (s) {
            char *p = s->mtrace_logbuf;
            int max_log_len = sizeof(s->mtrace_logbuf) - 1;
            p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, instruction: %s, ", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
            p += snprintf(p, max_log_len, "access vaddr: 0x%lx, len: %d", addr, len);
        }
#endif

#ifdef CONFIG_MTRACE_COND
        if (s && MTRACE_COND) {
            mtrace_log_write("%s\n", s->mtrace_logbuf);
        }
#endif
        int page_offset = addr & ((0x1lu << 12) - 1);
        if (len < PGSIZE - page_offset) {
            return paddr_read(s, isa_mmu_translate(addr, len, MEM_TYPE_READ), len);
        } else {
            int first = PGSIZE - page_offset;
            int rem = len + page_offset - PGSIZE;
            word_t ret = 0;
            paddr_t first_base = isa_mmu_translate(addr, first, MEM_TYPE_READ);
            paddr_t rem_base = isa_mmu_translate(addr + first, rem, MEM_TYPE_READ);
            for (int i = 0; i < first; i++) {
                ret |= (paddr_read(s, first_base + i, 1) << (8 * i));
            }
            for (int i = 0; i < rem; i++) {
                ret |= (paddr_read(s, rem_base + i, 1) << (8 * (i + first)));
            }
            return ret;
        }
    }
}

void vaddr_write(Decode *s, vaddr_t addr, int len, word_t data) {
    if (!isa_mmu_check(addr, len, MEM_TYPE_READ)) {
        paddr_write(s, addr, len, data);
    } else {
#ifdef CONFIG_MTRACE
        if (s) {
            char *p = s->mtrace_logbuf;
            int max_log_len = sizeof(s->mtrace_logbuf) - 1;
            p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, instruction: %s, ", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
            p += snprintf(p, max_log_len, "access vaddr: 0x%lx, len: %d, data: 0x%lx", addr, len, data);
        }
#endif

#ifdef CONFIG_MTRACE_COND
        if (s && MTRACE_COND) {
            mtrace_log_write("%s\n", s->mtrace_logbuf);
        }
#endif
        int page_offset = addr & ((0x1lu << 12) - 1);
        if (len < PGSIZE - page_offset) {
            paddr_write(s, isa_mmu_translate(addr, len, MEM_TYPE_WRITE), len, data);
        } else {
            int first = PGSIZE - page_offset;
            int rem = len + page_offset - PGSIZE;
            paddr_t first_base = isa_mmu_translate(addr, first, MEM_TYPE_WRITE);
            paddr_t rem_base = isa_mmu_translate(addr + first, rem, MEM_TYPE_WRITE);
            for (int i = 0; i < first; i++) {
                paddr_write(s, first_base + i, 1, (data >> (8 * i)));
            }
            for (int i = 0; i < rem; i++) {
                paddr_write(s, rem_base + i, 1, (data >> (8 * (i + first))));
            }
        }
    }
}
