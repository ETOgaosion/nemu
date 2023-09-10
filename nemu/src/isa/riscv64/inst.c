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
#include "monitor/symbol.h"
#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/ifetch.h>
// #include <pthread.h>

#define R(i) gpr(i)
#define RS(i) (int64_t) gpr(i)
#define R32(i) (uint32_t) gpr(i)
#define R32S(i) (int32_t) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
    TYPE_R,
    TYPE_I,
    TYPE_S,
    TYPE_B,
    TYPE_U,
    TYPE_J,
    TYPE_N, // none
};

static uint64_t inline sign_extend(uint32_t input, int length) {
    uint64_t ret = input;
    switch (length) {
    case 8:
        if (input & (0x1 << 7)) {
            ret |= (0xffffffffffffff00);
        }
        break;
    case 12:
        if (input & (0x1 << 11)) {
            ret |= (0xfffffffffffff000);
        }
        break;
    case 13:
        if (input & (0x1 << 12)) {
            ret |= (0xffffffffffffe000);
        }
        break;
    case 16:
        if (input & (0x1 << 15)) {
            ret |= (0xffffffffffff0000);
        }
        break;
    case 21:
        if (input & (0x1 << 20)) {
            ret |= (0xffffffffffe00000);
        }
        break;
    case 32:
        if (input & (0x1 << 31)) {
            ret |= (0xffffffff00000000);
        }
        break;
    default:
        panic("[sign extend] not supported length\n");
        break;
    }
    return ret;
}

#ifdef CONFIG_ITRACE
static void display_inst(Decode *s, int rd, int rs1, int rs2, int64_t imm, bool is_front) {
    char *p = NULL;
    int max_log_len;
    p = s->itrace_logbuf;
    max_log_len = sizeof(s->itrace_logbuf) - 1;
    if (is_front) {
        p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, inst name: %s; ", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
        p += snprintf(p, max_log_len, "rs1: ");
        if (rs1 >= 0 && rs1 < 32) {
            p += snprintf(p, max_log_len, "%s, rs1 value: 0x%lx; ", reg_name(rs1), R(rs1));
        } else {
            p += snprintf(p, max_log_len, "%d; ", rs1);
        }
        p += snprintf(p, max_log_len, "rs2: ");
        if (rs2 >= 0 && rs2 < 32) {
            p += snprintf(p, max_log_len, "%s, rs2 value: 0x%lx; ", reg_name(rs2), R(rs2));
        } else {
            p += snprintf(p, max_log_len, "%d; ", rs2);
        }
        p += snprintf(p, max_log_len, "imm: ");
        if (imm < 0) {
            p += snprintf(p, max_log_len, "%ld; ", imm);
        } else {
            p += snprintf(p, max_log_len, "0x%lx; ", (uint64_t)imm);
        }
        if (csr(imm)) {
            p += snprintf(p, max_log_len, "csr: %s, csr value: 0x%lx; ", csr_name(imm), *csr(imm));
        }
    } else {
        p += strlen(p);
        p += snprintf(p, max_log_len, "rd: ");
        if (rd >= 0 && rd < 32) {
            p += snprintf(p, max_log_len, "%s, rd value: 0x%lx; ", reg_name(rd), R(rd));
        } else {
            p += snprintf(p, max_log_len, "%d; ", rd);
        }
        if (csr(imm)) {
            p += snprintf(p, max_log_len, "csr: %s, csr value: 0x%lx; ", csr_name(imm), *csr(imm));
        }
    }
}
#endif

uint64_t g_nr_guest_inst_f;

#ifdef CONFIG_FTRACE
int function_depth = 0;
#endif

static void display_ftrace(Decode *s, int rd, int rs1, int rs2, int64_t imm) {
#ifdef CONFIG_FTRACE
    g_nr_guest_inst_f++;
    char *p = NULL;
    int max_log_len;
    p = s->ftrace_logbuf;
    max_log_len = sizeof(s->ftrace_logbuf) - 1;
    if (strncmp(s->isa.inst.name, "jal", 3) == 0) {
        function_symbol_t *func = find_function_symbol(s->dnpc, false);
        if (func) {
            // call
            p += snprintf(p, max_log_len, "[%lld] 0x%lx: ", s->count, s->pc);
            for (int i = 0; i < function_depth && i < max_log_len / 4; i++) {
                p += snprintf(p, max_log_len, "  ");
            }
            p += snprintf(p, max_log_len, "call [%s@0x%lx@%s]", func->name, func->address, func->file_name);
            function_depth++;
        } else if (s->isa.inst.name[3] == 'r' && rd == reg_zero && function_depth > 0) {
            // ret
            func = find_function_symbol(s->pc, true);
            function_depth--;
            p += snprintf(p, max_log_len, "[%lld] 0x%lx: ", s->count, s->pc);
            for (int i = 0; i < function_depth && i < max_log_len / 4; i++) {
                p += snprintf(p, max_log_len, "  ");
            }
            p += snprintf(p, max_log_len, "ret 0x%lx [0x%lx@%s@%s]", gpr(reg_a0), s->dnpc, func->name, func->file_name);
        }
    }
#endif
}

static void inline decode_operand(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm, int type) {
    *rs1 = BITS(inst, 19, 15);
    *rs2 = BITS(inst, 24, 20);
    *rd = BITS(inst, 11, 7);
    switch (type) {
    case TYPE_I:
        *imm = sign_extend(BITS(inst, 31, 20), 12);
        break;
    case TYPE_S:
        *imm = sign_extend((BITS(inst, 31, 25) << 5) | (BITS(inst, 11, 7)), 12);
        break;
    case TYPE_B:
        *imm = sign_extend((BITS(inst, 31, 31) << 12) | (BITS(inst, 7, 7) << 11) | (BITS(inst, 30, 25) << 5) | (BITS(inst, 11, 8) << 1), 13);
        break;
    case TYPE_U:
        *imm = sign_extend((BITS(inst, 31, 12) << 12), 32);
        break;
    case TYPE_J:
        *imm = sign_extend((BITS(inst, 31, 31) << 20) | (BITS(inst, 19, 12) << 12) | (BITS(inst, 20, 20) << 11) | (BITS(inst, 30, 21) << 1), 21);
        break;

    default:
        break;
    }

#ifdef CONFIG_ITRACE
    display_inst(s, *rd, *rs1, *rs2, *imm, true);
#endif
}

static void fence_op(int imm, bool inst) {}

#if defined CONFIG_ITRACE || defined OWN_DECODE
#define opcode_mask 0x7f
#define midcode_mask 0x7000
#endif

static int decode_exec(Decode *s) {
    int rd = 0, rs1 = 0, rs2 = 0;
    int64_t imm = 0;
    s->dnpc = s->snpc;

    /* clang-format off */
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#if defined CONFIG_ITRACE || defined CONFIG_MTRACE
#define INSTPAT_MATCH(s, inst_name, type, ... /* execute body */ ) { \
    strcpy(s->isa.inst.name, inst_name); \
    decode_operand(s, s->isa.inst.val, &rd, &rs1, &rs2, &imm, concat(TYPE_, type)); \
    __VA_ARGS__ ; \
}
#else
#define INSTPAT_MATCH(s, inst_name, type, ... /* execute body */ ) { \
    decode_operand(s, s->isa.inst.val, &rd, &rs1, &rs2, &imm, concat(TYPE_, type)); \
    __VA_ARGS__ ; \
}
#endif

    INSTPAT_START();

    #include "generated/inst.h"

    INSTPAT("??????? ????? ????? ??? ????? ???????", "inv", N, INV(s->pc));
    INSTPAT_END();

    /* clang-format on */

    R(reg_zero) = 0; // reset $zero to 0

#ifdef CONFIG_ITRACE
    display_inst(s, rd, rs1, rs2, imm, false);
#endif

    return 0;
}

int isa_exec_once(Decode *s) {
    s->isa.inst.val = inst_fetch(&s->snpc, 4);
    return decode_exec(s);
}