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
static void display_inst(Decode *s, uint32_t mid_op, int rd, int rs1, int rs2, int64_t imm, bool is_front) {
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
        p += snprintf(p, max_log_len, "mid_op: 0x%x; ", mid_op);
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
    display_inst(s, 0, *rd, *rs1, *rs2, *imm, true);
#endif
}

static void fence_op(int imm, bool inst) {}


#ifdef OWN_DECODE

/* All riscv opcodes last 2 bytes are 11, no need to compare */
/* clang-format off */
/*                    ---|||||*/
#define MID_OP_LB       0b00000000
#define MID_OP_LH       0b00100000
#define MID_OP_LW       0b01000000
#define MID_OP_LD       0b01100000
#define MID_OP_LBU      0b10000000
#define MID_OP_LHU      0b10100000
#define MID_OP_LWU      0b11000000
#define MID_OP_FENCE    0b00000011
#define MID_OP_FENCE_I  0b00100011
#define MID_OP_ADDI     0b00000100
#define MID_OP_SLLI     0b00100100
#define MID_OP_SLTI     0b01000100
#define MID_OP_SLTIU    0b01100100
#define MID_OP_XORI     0b10000100
#define MID_OP_SRLI     0b10100100  // SRAI
#define MID_OP_ORI      0b11000100
#define MID_OP_ANDI     0b11100100
#define MID_OP_AUIPC_0  0b00000101
#define MID_OP_AUIPC_1  0b00100101
#define MID_OP_AUIPC_2  0b01000101
#define MID_OP_AUIPC_3  0b01100101
#define MID_OP_AUIPC_4  0b10000101
#define MID_OP_AUIPC_5  0b10100101
#define MID_OP_AUIPC_6  0b11000101
#define MID_OP_AUIPC_7  0b11100101
#define MID_OP_ADDIW    0b00000110
#define MID_OP_SLLIW    0b00100110
#define MID_OP_SRLIW    0b10100110  // SRAIW
#define MID_OP_SB       0b00001000
#define MID_OP_SH       0b00101000
#define MID_OP_SW       0b01001000
#define MID_OP_SD       0b01101000
#define MID_OP_ADD      0b00001100  // SUB, MUL
#define MID_OP_SLL      0b00101100  // MULH
#define MID_OP_SLT      0b01001100  // MULHSU
#define MID_OP_SLTU     0b01101100  // MULHU
#define MID_OP_XOR      0b10001100  // DIV
#define MID_OP_SRL      0b10101100  // SRA, DIVU
#define MID_OP_OR       0b11001100  // REM
#define MID_OP_AND      0b11101100  // REMU
#define MID_OP_LUI_0    0b00001101
#define MID_OP_LUI_1    0b00101101
#define MID_OP_LUI_2    0b01001101
#define MID_OP_LUI_3    0b01101101
#define MID_OP_LUI_4    0b10001101
#define MID_OP_LUI_5    0b10101101
#define MID_OP_LUI_6    0b11001101
#define MID_OP_LUI_7    0b11101101
#define MID_OP_ADDW     0b00001110  // SUBW, MULW
#define MID_OP_SLLW     0b00101110
#define MID_OP_DIVW     0b10001110
#define MID_OP_SRLW     0b10101110  // SRAW, DIVUW
#define MID_OP_REMW     0b11001110
#define MID_OP_REMUW    0b11101110
#define MID_OP_BEQ      0b00011000
#define MID_OP_BNE      0b00111000
#define MID_OP_BLT      0b10011000
#define MID_OP_BGE      0b10111000
#define MID_OP_BLTU     0b11011000
#define MID_OP_BGEU     0b11111000
#define MID_OP_JALR     0b00011001
#define MID_OP_JAL_0    0b000011011
#define MID_OP_JAL_1    0b00111011
#define MID_OP_JAL_2    0b01011011
#define MID_OP_JAL_3    0b01111011
#define MID_OP_JAL_4    0b10011011
#define MID_OP_JAL_5    0b10111011
#define MID_OP_JAL_6    0b11011011
#define MID_OP_JAL_7    0b11111011
#define MID_OP_ECALL    0b00011100  // ebreak sret mret
#define MID_OP_CSRRW    0b00111100
#define MID_OP_CSRRS    0b01011100
#define MID_OP_CSRRC    0b01111100
#define MID_OP_CSRRWI   0b10111100
#define MID_OP_CSRRSI   0b11011100
#define MID_OP_CSRRCI   0b11111100

#define opcode_mask 0x7f
#define midcode_mask 0x7000
#define FRONTCODE7_1 0x1 << 25
#define FRONTCODE7_6 0x1 << 30
#define FRONTCODE12_1 0x1 << 20
#define FRONTCODE12_2 0x1 << 21
#define FRONTCODE12_9 0x1 << 28
#define FRONTCODE12_10 0x1 << 29
/* clang-format on */

typedef void (*decode_handler)(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm);

#include "generated/decode_operation.h"
#include "generated/handle_decode_conflicts.h"

decode_handler decode_table[253] = {
    [MID_OP_LB] = riscv64_lb,
    [MID_OP_LH] = riscv64_lh,
    [MID_OP_LW] = riscv64_lw,
    [MID_OP_LD] = riscv64_ld,
    [MID_OP_LBU] = riscv64_lbu,
    [MID_OP_LHU] = riscv64_lhu,
    [MID_OP_LWU] = riscv64_lwu,
    [MID_OP_FENCE] = riscv64_fence,
    [MID_OP_FENCE_I] = riscv64_fencei,
    [MID_OP_ADDI] = riscv64_addi,
    [MID_OP_SLLI] = riscv64_slli,
    [MID_OP_SLTI] = riscv64_slti,
    [MID_OP_SLTIU] = riscv64_sltiu,
    [MID_OP_XORI] = riscv64_xori,
    [MID_OP_SRLI] = handle_srli_srai,
    [MID_OP_ORI] = riscv64_ori,
    [MID_OP_ANDI] = riscv64_andi,
    [MID_OP_AUIPC_0] = riscv64_auipc,
    [MID_OP_AUIPC_1] = riscv64_auipc,
    [MID_OP_AUIPC_2] = riscv64_auipc,
    [MID_OP_AUIPC_3] = riscv64_auipc,
    [MID_OP_AUIPC_4] = riscv64_auipc,
    [MID_OP_AUIPC_5] = riscv64_auipc,
    [MID_OP_AUIPC_6] = riscv64_auipc,
    [MID_OP_AUIPC_7] = riscv64_auipc,
    [MID_OP_ADDIW] = riscv64_addiw,
    [MID_OP_SLLIW] = riscv64_slliw,
    [MID_OP_SRLIW] = handle_srliw_sraiw,
    [MID_OP_SB] = riscv64_sb,
    [MID_OP_SH] = riscv64_sh,
    [MID_OP_SW] = riscv64_sw,
    [MID_OP_SD] = riscv64_sd,
    [MID_OP_ADD] = handle_add_sub_mul,
    [MID_OP_SLL] = handle_sll_mulh,
    [MID_OP_SLT] = handle_slt_mulhsu,
    [MID_OP_SLTU] = handle_sltu_mulhu,
    [MID_OP_XOR] = handle_xor_div,
    [MID_OP_SRL] = handle_srl_sra_divu,
    [MID_OP_OR] = handle_or_rem,
    [MID_OP_AND] = handle_and_remu,
    [MID_OP_LUI_0] = riscv64_lui,
    [MID_OP_LUI_1] = riscv64_lui,
    [MID_OP_LUI_2] = riscv64_lui,
    [MID_OP_LUI_3] = riscv64_lui,
    [MID_OP_LUI_4] = riscv64_lui,
    [MID_OP_LUI_5] = riscv64_lui,
    [MID_OP_LUI_6] = riscv64_lui,
    [MID_OP_LUI_7] = riscv64_lui,
    [MID_OP_ADDW] = handle_addw_subw_mulw,
    [MID_OP_SLLW] = riscv64_sllw,
    [MID_OP_DIVW] = riscv64_divw,
    [MID_OP_SRLW] = handle_srlw_sraw_divuw,
    [MID_OP_REMW] = riscv64_remw,
    [MID_OP_REMUW] = riscv64_remuw,
    [MID_OP_BEQ] = riscv64_beq,
    [MID_OP_BNE] = riscv64_bne,
    [MID_OP_BLT] = riscv64_blt,
    [MID_OP_BGE] = riscv64_bge,
    [MID_OP_BLTU] = riscv64_bltu,
    [MID_OP_BGEU] = riscv64_bgeu,
    [MID_OP_JALR] = riscv64_jalr,
    [MID_OP_JAL_0] = riscv64_jal,
    [MID_OP_JAL_1] = riscv64_jal,
    [MID_OP_JAL_2] = riscv64_jal,
    [MID_OP_JAL_3] = riscv64_jal,
    [MID_OP_JAL_4] = riscv64_jal,
    [MID_OP_JAL_5] = riscv64_jal,
    [MID_OP_JAL_6] = riscv64_jal,
    [MID_OP_JAL_7] = riscv64_jal,
    [MID_OP_ECALL] = handle_ecall_ebreak_sret_mret,
    [MID_OP_CSRRW] = riscv64_csrrw,
    [MID_OP_CSRRS] = riscv64_csrrs,
    [MID_OP_CSRRC] = riscv64_csrrc,
    [MID_OP_CSRRWI] = riscv64_csrrwi,
    [MID_OP_CSRRSI] = riscv64_csrrsi,
    [MID_OP_CSRRCI] = riscv64_csrrci,
};

#endif


static int decode_exec(Decode *s) {
    int rd = 0, rs1 = 0, rs2 = 0;
    int64_t imm = 0;
    s->dnpc = s->snpc;

#ifndef OWN_DECODE
    /* clang-format off */
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, inst_name, type, ... /* execute body */ ) { \
    decode_operand(s, s->isa.inst.val, &rd, &rs1, &rs2, &imm, concat(TYPE_, type)); \
    __VA_ARGS__ ; \
}

    INSTPAT_START();

    #include "generated/inst.h"

    INSTPAT("??????? ????? ????? ??? ????? ???????", "inv", N, INV(s->pc));
    INSTPAT_END();

/* clang-format on */
#else
    uint32_t inst = s->isa.inst.val;
    uint32_t mid_op = ((inst & midcode_mask) >> 7) | ((inst & opcode_mask) >> 2);
    decode_table[mid_op](s, inst, &rd, &rs1, &rs2, &imm);
#endif

    R(reg_zero) = 0; // reset $zero to 0

#ifdef CONFIG_ITRACE
    display_inst(s, mid_op, rd, rs1, rs2, imm, false);
#endif

    return 0;
}

int isa_exec_once(Decode *s) {
    s->isa.inst.val = inst_fetch(&s->snpc, 4);
    return decode_exec(s);
}