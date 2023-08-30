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

#ifndef __RISCV64_REG_H__
#define __RISCV64_REG_H__

#include <common.h>

#define reg_zero 0
#define reg_ra 1
#define reg_sp 2
#define reg_gp 3
#define reg_tp 4
#define reg_t0 5
#define reg_t1 6
#define reg_t2 7
#define reg_s0 8
#define reg_s1 9
#define reg_a0 10
#define reg_a1 11
#define reg_a2 12
#define reg_a3 13
#define reg_a4 14
#define reg_a5 15
#define reg_a6 16
#define reg_a7 17
#define reg_s2 18
#define reg_s3 19
#define reg_s4 20
#define reg_s5 21
#define reg_s6 22
#define reg_s7 23
#define reg_s8 24
#define reg_s9 25
#define reg_s10 26
#define reg_s11 27
#define reg_t3 28
#define reg_t4 29
#define reg_t5 30
#define reg_t6 31

#define csr_mstatus 0x300
#define csr_mtvec 0x305
#define csr_mepc 0x341
#define csr_mcause 0x342

static inline int check_reg_idx(int idx) {
    IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
    return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])

static inline const char *reg_name(int idx) {
    extern const char *regs[];
    return regs[check_reg_idx(idx)];
}

word_t *csr(int num);
const char *csr_name(int num);

#endif
