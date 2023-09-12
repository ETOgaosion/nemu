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

#include <cpu/decode.h>
#include <isa.h>

uint64_t g_nr_guest_inst_e;

void isa_raise_intr(Decode *s, word_t NO) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
     * Then return the address of the interrupt/exception vector.
     */
    if (NO == ECALL) {
        if (cpu.mstatus & MSTATUS_MPP_BITS) {
            cpu.mcause = ECALL_M;
        } else {
            cpu.mcause = ECALL_U;
        }
    } else {
        cpu.mcause = NO;
    }
    cpu.mepc = s->pc;
    cpu.mstatus |= MSTATUS_MPP_BITS;
    cpu.mstatus |= (cpu.mstatus & MSTATUS_MIE_BITS) << 4;
    cpu.mstatus &= ~MSTATUS_MIE_BITS;
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
    cpu.mstatus &= ~MSTATUS_MPP_BITS;
    cpu.mstatus |= (cpu.mstatus & MSTATUS_MPIE_BITS) >> 4;
    cpu.mstatus |= MSTATUS_MPIE_BITS;
    return cpu.mepc;
}

word_t isa_query_intr() {
    if (cpu.INTR && (cpu.mstatus & MSTATUS_MIE_BITS)) {
        cpu.INTR = false;
        return IRQ_TIMER;
    }
    else {
        return INTR_EMPTY;
    }
}
