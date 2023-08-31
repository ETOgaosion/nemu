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
        p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x; ", s->count, s->pc, s->isa.inst.val);
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
    }
    else {
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

static void inline decode_operand(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm, int type) {
    uint32_t i = s->isa.inst.val;
    *rs1 = BITS(i, 19, 15);
    *rs2 = BITS(i, 24, 20);
    *rd = BITS(i, 11, 7);
    switch (type) {
    case TYPE_I:
        *imm = sign_extend(BITS(i, 31, 20), 12);
        break;
    case TYPE_S:
        *imm = sign_extend((BITS(i, 31, 25) << 5) | (BITS(i, 11, 7)), 12);
        break;
    case TYPE_B:
        *imm = sign_extend((BITS(i, 31, 31) << 12) | (BITS(i, 7, 7) << 11) | (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1), 13);
        break;
    case TYPE_U:
        *imm = sign_extend((BITS(i, 31, 12) << 12), 32);
        break;
    case TYPE_J:
        *imm = sign_extend((BITS(i, 31, 31) << 20) | (BITS(i, 19, 12) << 12) | (BITS(i, 20, 20) << 11) | (BITS(i, 30, 21) << 1), 21);
        break;

    default:
        break;
    }

#ifdef CONFIG_ITRACE
    display_inst(s, *rd, *rs1, *rs2, *imm, true);
#endif
}

static void fence_op(int imm, bool inst) {}

/* All riscv opcodes last 2 bytes are 11, no need to compare */
/* clang-format off */
#define OPCODE_R_AR    0b01100
#define OPCODE_R_AR64  0b01110
#define OPCODE_I_J     0b11001
#define OPCODE_I_LD    0b00000
#define OPCODE_I_AR    0b00100
#define OPCODE_I_AR64  0b00110
#define OPCODE_I_FENCE 0b00011
#define OPCODE_I_PR    0b11100
#define OPCODE_S       0b01000
#define OPCODE_B       0b11000
#define OPCODE_U_L     0b01101
#define OPCODE_U_A     0b00101
#define OPCODE_J       0b11011

#define MIDCODE_0 0b000
#define MIDCODE_1 0b001
#define MIDCODE_2 0b010
#define MIDCODE_3 0b011
#define MIDCODE_4 0b100
#define MIDCODE_5 0b101
#define MIDCODE_6 0b110
#define MIDCODE_7 0b111

#define FRONTCODE_0         0
#define FRONTCODE6_5        0b010000
#define FRONTCODE7_1        0b0000001
#define FRONTCODE7_6        0b0100000
#define FRONTCODE12_1       0b000000000001
#define FRONTCODE12_9_2     0b000100000010
#define FRONTCODE12_10_9_2  0b001100000010

/*               -------|||*/
#define ADD    0b0000000000
#define SUB    0b0100000000
#define MUL    0b0000001000
#define SLL    0b0000000001
#define MULH   0b0000001001
#define SLT    0b0000000010
#define MULHSU 0b0000001010
#define SLTU   0b0000000011
#define MULHU  0b0000001011
#define XOR    0b0000000100
#define DIV    0b0000001100
#define SRL    0b0000000101
#define SRA    0b0100000101
#define DIVU   0b0000001101
#define OR     0b0000000110
#define REM    0b0000001110
#define AND    0b0000000111
#define REMU   0b0000001111

#define opcode_mask 0x1f
#define midcode_mask 0x7
/* clang-format on */

static int decode_exec(Decode *s) {
    int rd = 0, rs1 = 0, rs2 = 0;
    int64_t imm = 0;
    s->dnpc = s->snpc;

#ifdef FRAME_DECODE
    /* clang-format off */
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, inst_name, type, ... /* execute body */ ) { \
    decode_operand(s, &rd, &rs1, &rs2, &imm, concat(TYPE_, type)); \
    __VA_ARGS__ ; \
}

    INSTPAT_START();

    #include "generated/inst.h"

    INSTPAT("??????? ????? ????? ??? ????? ???????", "inv", N, INV(s->pc));
    INSTPAT_END();

/* clang-format on */
#else
    #include "generated/own_decoder.h"
#endif

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