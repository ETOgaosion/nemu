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

#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>

typedef struct {
    word_t gpr[32];
    vaddr_t pc;
    vaddr_t last_pc;
    word_t mstatus;
    word_t mtvec;
    word_t mepc;
    word_t mcause;
    word_t satp;
} riscv64_CPU_state;

#define MSTATUS_MPP_BITS 0x1800
#define MSTATUS_SUM_BITS 0x40000
#define MSTATUS_MXR_BITS 0x80000

// decode
typedef struct {
    struct {
        uint32_t val;
        char name[7];
    } inst;
} riscv64_ISADecodeInfo;

typedef uint64_t PTE;

/*
 * PTE format:
 * | XLEN-1  10 | 9             8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
 *       PFN      reserved for SW   D   A   G   U   X   W   R   V
 */

#define _PAGE_ACCESSED_OFFSET 6

#define _PAGE_PRESENT ((uint64_t)1 << 0)
#define _PAGE_READ ((uint64_t)1 << 1)   /* Readable */
#define _PAGE_WRITE ((uint64_t)1 << 2)  /* Writable */
#define _PAGE_EXEC ((uint64_t)1 << 3)   /* Executable */
#define _PAGE_USER ((uint64_t)1 << 4)   /* User */
#define _PAGE_GLOBAL ((uint64_t)1 << 5) /* Global */
#define _PAGE_ACCESSED (1 << 6)         /* Set by hardware on any access */
#define _PAGE_DIRTY ((uint64_t)1 << 7)  /* Set by hardware on any write */
#define _PAGE_SOFT ((uint64_t)1 << 8)   /* Reserved for software */

#define _PAGE_PFN_SHIFT 10lu
#define _PAGE_ADDR_SHIFT 12lu

#define PGSIZE 4096

int isa_mmu_check(word_t vaddr, int len, int type);

#endif
