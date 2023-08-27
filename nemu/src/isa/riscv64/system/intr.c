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
#include <cpu/decode.h>

uint64_t g_nr_guest_inst_e;

void isa_raise_intr(Decode *s, word_t NO) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
     * Then return the address of the interrupt/exception vector.
     */
    cpu.mcause = NO;
    cpu.mepc = s->pc;
    cpu.mstatus = cpu.mstatus | (0x1 << 17);
    g_nr_guest_inst_e++;

#ifdef CONFIG_ETRACE
    char *p = s->etrace_logbuf;
    int max_log_len = sizeof(s->etrace_logbuf) - 1;
    p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, instruction: %s, ", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
    p += snprintf(p, max_log_len, "interrupt number: %ld", NO);
#endif

#ifdef CONFIG_ETRACE_COND
    if (ETRACE_COND) {
        etrace_log_write("%s\n", s->etrace_logbuf);
    }
#endif

    s->dnpc = cpu.mtvec;
}

vaddr_t isa_ret_intr() {
    cpu.mstatus = cpu.mstatus & (~(0x1 << 17));
    return cpu.mepc;
}

word_t isa_query_intr() {
    return INTR_EMPTY;
}
