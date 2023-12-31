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
#include <memory/vaddr.h>

int isa_mmu_check(word_t vaddr, int len, int type) {
    if (cpu.satp & (0x1lu << 63)) {
        return MMU_TRANSLATE;
    } else if (cpu.satp) {
        return MMU_FAIL;
    } else {
        return MMU_DIRECT;
    }
}

static inline uint64_t get_pa(PTE entry) {
    return (entry >> _PAGE_PFN_SHIFT) << _PAGE_ADDR_SHIFT;
}

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
    word_t pgdir = ((cpu.satp) << 20) >> 8;
    uint64_t vpn[] = {
        (vaddr >> 12) & ((0x1lu << 9) - 1), // vpn0
        (vaddr >> 21) & ((0x1lu << 9) - 1), // vpn1
        (vaddr >> 30) & ((0x1lu << 9) - 1)  // vpn2
    };
    PTE *page_base = (uint64_t *)pgdir;
    /* second page */
    PTE entry = paddr_read(NULL, (uint64_t)page_base + (vpn[2] << 3), 8);
    PTE *second_page = (uint64_t *)get_pa(entry);
    /* finally page */
    entry = paddr_read(NULL, (uint64_t)second_page + (vpn[1] << 3), 8);
    PTE *third_page = (uint64_t *)get_pa(entry);
    /* physical addr */
    entry = paddr_read(NULL, (uint64_t)third_page + (vpn[0] << 3), 8);
    paddr_t ret = (get_pa(entry) | (vaddr & ((0x1lu << 12) - 1)));
    return ret;
}
