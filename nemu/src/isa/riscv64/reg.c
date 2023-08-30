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

#include "local-include/reg.h"
#include <isa.h>

const char *regs[] = {"$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void isa_reg_display() {
    for (int i = 0; i < 16; i++) {
        printf("%s: 0x%16lx, %s: 0x%16lx;\n", regs[2 * i], cpu.gpr[2 * i], regs[2 * i + 1], cpu.gpr[2 * i + 1]);
    }
    printf("pc: 0x%16lx\n", cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
    if (strcmp(s, "pc") == 0) {
        return cpu.pc;
    } else {
        for (int i = 0; i < 32; i++) {
            if (strcmp(s, regs[i]) == 0) {
                return cpu.gpr[i];
            }
        }
    }
    return 0;
}

word_t *csr(int num) {
    switch (num) {
        case csr_mstatus:
            return &cpu.mstatus;
            break;
        case csr_mtvec:
            return &cpu.mtvec;
            break;
        case csr_mepc:
            return &cpu.mepc;
            break;
        case csr_mcause:
            return &cpu.mcause;
            break;
        default:
            panic("Not supported csr register: %d", num);
            break;
    }
    return 0;
}

const char *csr_name(int num) {
    switch (num) {
        case csr_mstatus:
            return "mstatus";
            break;
        case csr_mtvec:
            return "mtvec";
            break;
        case csr_mepc:
            return "mepc";
            break;
        case csr_mcause:
            return "mcause";
            break;
        default:
            panic("Not supported csr register: %d", num);
            break;
    }
    return NULL;
}