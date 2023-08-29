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
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include "monitor/symbol.h"

#define R(i) gpr(i)
#define RS(i) (int64_t)gpr(i)
#define R32(i) (uint32_t)gpr(i)
#define R32S(i) (int32_t)gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_R, TYPE_I, TYPE_S, TYPE_B, TYPE_U, TYPE_J,
  TYPE_N, // none
};

static uint64_t sign_extend(uint32_t input, int length) {
  uint64_t ret = input;
  switch (length)
  {
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

#ifdef CONFIG_FTRACE
int function_depth = 0;
#endif

static void display_inst(Decode *s, int rd, int rs1, int rs2, int64_t imm) {
#ifdef CONFIG_ITRACE
  char *p = NULL;
  int max_log_len;
  p = s->itrace_logbuf;
  max_log_len = sizeof(s->itrace_logbuf) - 1;
  p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, instruction: %s\n", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
  p += snprintf(p, max_log_len, "rs1: ");
  if (rs1 >= 0 && rs1 < 32) {
    p += snprintf(p, max_log_len, "%s, rs1 value: 0x%lx\n", reg_name(rs1), R(rs1));
  }
  else {
    p += snprintf(p, max_log_len, "%d\n", rs1);
  }
  p += snprintf(p, max_log_len, "rs2: ");
  if (rs2 >= 0 && rs2 < 32) {
    p += snprintf(p, max_log_len, "%s, rs2 value: 0x%lx\n", reg_name(rs2), R(rs2));
  }
  else {
    p += snprintf(p, max_log_len, "%d\n", rs2);
  }
  p += snprintf(p, max_log_len, "rd: ");
  if (rd >= 0 && rd < 32) {
    p += snprintf(p, max_log_len, "%s, rd value: 0x%lx\n", reg_name(rd), R(rd));
  }
  else {
    p += snprintf(p, max_log_len, "%d\n", rd);
  }
  p += snprintf(p, max_log_len, "imm: ");
  if (imm < 0) {
    p += snprintf(p, max_log_len, "%ld\n", imm);
  }
  else {
    p += snprintf(p, max_log_len, "0x%lx\n", (uint64_t)imm);
  }
#endif
}

uint64_t g_nr_guest_inst_f;

static void display_ftrace(Decode *s, int rd, int rs1, int rs2, int64_t imm) {
    g_nr_guest_inst_f++;
#ifdef CONFIG_FTRACE
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
    }
    else if (s->isa.inst.name[3] == 'r' && rd == reg_zero && function_depth > 0) {
      // ret
      function_depth--;
      p += snprintf(p, max_log_len, "[%lld] 0x%lx: ", s->count, s->pc);
      for (int i = 0; i < function_depth && i < max_log_len / 4; i++) {
        p += snprintf(p, max_log_len, "  ");
      }
      func = find_function_symbol(s->pc, true);
      p += snprintf(p, max_log_len, "ret [%s@%s]", func->name, func->file_name);
    }
  }
#endif
}

static void decode_operand(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  *rs1 = BITS(i, 19, 15);
  *rs2 = BITS(i, 24, 20);
  *rd  = BITS(i, 11, 7);
  switch (type)
  {
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
}

static void fence_op(int imm, bool inst) {}

static int decode_exec(Decode *s) {
  int rd = 0, rs1 = 0, rs2 = 0;
  int64_t imm = 0;
  s->dnpc = s->snpc;

/* clang-format off */
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, inst_name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &rs1, &rs2, &imm, concat(TYPE_, type)); \
  memset((void *)s->isa.inst.name, 0, sizeof(s->isa.inst.name)); \
  memcpy((void *)s->isa.inst.name, inst_name, strlen(inst_name)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  // R Type
  // riscv32
  INSTPAT("0000000 ????? ????? 000 ????? 0110011", "add", R, R(rd) = R(rs1) + R(rs2));
  INSTPAT("0100000 ????? ????? 000 ????? 0110011", "sub", R, R(rd) = R(rs1) - R(rs2));
  INSTPAT("0000000 ????? ????? 001 ????? 0110011", "sll", R, R(rd) = R(rs1) << (R(rs2) & 0b111111));
  INSTPAT("0000000 ????? ????? 010 ????? 0110011", "slt", R, R(rd) = RS(rs1) < RS(rs2));
  INSTPAT("0000000 ????? ????? 011 ????? 0110011", "sltu", R, R(rd) = R(rs1) < R(rs2));
  INSTPAT("0000000 ????? ????? 100 ????? 0110011", "xor", R, R(rd) = R(rs1) ^ R(rs2));
  INSTPAT("0000000 ????? ????? 101 ????? 0110011", "srl", R, R(rd) = R(rs1) >> (R(rs2) & 0b111111));
  INSTPAT("0100000 ????? ????? 101 ????? 0110011", "sra", R, R(rd) = RS(rs1) >> (R(rs2) & 0b111111));
  INSTPAT("0000000 ????? ????? 110 ????? 0110011", "or", R, R(rd) = R(rs1) | R(rs2));
  INSTPAT("0000000 ????? ????? 111 ????? 0110011", "and", R, R(rd) = R(rs1) & R(rs2));
  // riscv32m
  INSTPAT("0000001 ????? ????? 000 ????? 0110011", "mul", R, R(rd) = R(rs1) * R(rs2));
  INSTPAT("0000001 ????? ????? 001 ????? 0110011", "mulh", R, R(rd) = (word_t)(((__int128_t)R(rs1) * (__int128_t)R(rs2)) >> 64));
  INSTPAT("0000001 ????? ????? 010 ????? 0110011", "mulhsu", R, R(rd) = (word_t)(((__int128_t)R(rs1) * (__uint128_t)R(rs2)) >> 64));
  INSTPAT("0000001 ????? ????? 011 ????? 0110011", "mulhu", R, R(rd) = (word_t)(((__uint128_t)R(rs1) * (__uint128_t)R(rs2)) >> 64));
  INSTPAT("0000001 ????? ????? 100 ????? 0110011", "div", R, R(rd) = RS(rs1) / RS(rs2));
  INSTPAT("0000001 ????? ????? 101 ????? 0110011", "divu", R, R(rd) = R(rs1) / R(rs2));
  INSTPAT("0000001 ????? ????? 110 ????? 0110011", "rem", R, R(rd) = RS(rs1) % RS(rs2));
  INSTPAT("0000001 ????? ????? 111 ????? 0110011", "remu", R, R(rd) = R(rs1) % R(rs2));

  // riscv64
  INSTPAT("0000000 ????? ????? 000 ????? 0111011", "addw", R, R(rd) = sign_extend(R(rs1) + R(rs2), 32));
  INSTPAT("0100000 ????? ????? 000 ????? 0111011", "subw", R, R(rd) = sign_extend(R(rs1) - R(rs2), 32));
  INSTPAT("0000000 ????? ????? 001 ????? 0111011", "sllw", R, R(rd) = sign_extend(R(rs1) << (R(rs2) & 0b111111), 32));
  INSTPAT("0000000 ????? ????? 101 ????? 0111011", "srlw", R, R(rd) = sign_extend(R32(rs1) >> (R(rs2) & 0b111111), 32));
  INSTPAT("0100000 ????? ????? 101 ????? 0111011", "sraw", R, R(rd) = sign_extend(R32S(rs1) >> (R(rs2) & 0b111111), 32));
  // riscv64m
  INSTPAT("0000001 ????? ????? 000 ????? 0111011", "mulw", R, R(rd) = sign_extend(R(rs1) * R(rs2), 32));
  INSTPAT("0000001 ????? ????? 100 ????? 0111011", "divw", R, R(rd) = sign_extend(R32S(rs1) / R32S(rs2), 32));
  INSTPAT("0000001 ????? ????? 101 ????? 0111011", "divuw", R, R(rd) = sign_extend(R32(rs1) / R32(rs2), 32));
  INSTPAT("0000001 ????? ????? 110 ????? 0111011", "remw", R, R(rd) = sign_extend(R32S(rs1) % R32S(rs2), 32));
  INSTPAT("0000001 ????? ????? 111 ????? 0111011", "remuw", R, R(rd) = sign_extend(R32(rs1) % R32(rs2), 32));

  // I Type
  // riscv32
  INSTPAT("??????? ????? ????? 000 ????? 1100111", "jalr", I, {s->dnpc = (R(rs1) + imm) & ~(uint64_t)0x1; R(rd) = s->pc + 4; display_ftrace(s, rd, rs1, rs2, imm);});

  INSTPAT("??????? ????? ????? 000 ????? 0000011", "lb", I, R(rd) = sign_extend(Mr(s, R(rs1) + imm, 1), 8));
  INSTPAT("??????? ????? ????? 001 ????? 0000011", "lh", I, R(rd) = sign_extend(Mr(s, R(rs1) + imm, 2), 16));
  INSTPAT("??????? ????? ????? 010 ????? 0000011", "lw", I, R(rd) = sign_extend(Mr(s, R(rs1) + imm, 4), 32));
  INSTPAT("??????? ????? ????? 100 ????? 0000011", "lbu", I, R(rd) = Mr(s, R(rs1) + imm, 1));
  INSTPAT("??????? ????? ????? 101 ????? 0000011", "lhu", I, R(rd) = Mr(s, R(rs1) + imm, 2));

  INSTPAT("??????? ????? ????? 000 ????? 0010011", "addi", I, R(rd) = R(rs1) + imm);
  INSTPAT("??????? ????? ????? 010 ????? 0010011", "slti", I, R(rd) = RS(rs1) < imm);
  INSTPAT("??????? ????? ????? 011 ????? 0010011", "sltiu", I, R(rd) = R(rs1) < (uint64_t)imm);
  INSTPAT("??????? ????? ????? 100 ????? 0010011", "xori", I, R(rd) = R(rs1) ^ imm);
  INSTPAT("??????? ????? ????? 110 ????? 0010011", "ori", I, R(rd) = R(rs1) | imm);
  INSTPAT("??????? ????? ????? 111 ????? 0010011", "andi", I, R(rd) = R(rs1) & imm);

  /* !-- WTF? riscv64 and riscv32 have different formats defined, but not shown in riscvg --! */
  INSTPAT("000000 ?????? ????? 001 ????? 0010011", "slli", I, R(rd) = R(rd) = R(rs1) << (imm & 0b111111));
  INSTPAT("000000 ?????? ????? 101 ????? 0010011", "srli", I, R(rd) = R(rs1) >> (imm & 0b111111));
  INSTPAT("010000 ?????? ????? 101 ????? 0010011", "srai", I, R(rd) = RS(rs1) >> (imm & 0b111111));

  INSTPAT("0000 ???? ???? 00000 000 00000 0001111", "fence", I, fence_op(imm, false));
  INSTPAT("0000 0000 0000 00000 001 00000 0001111", "fence_I", N, fence_op(0, true));

  INSTPAT("000000000000 00000 000 00000 1110011", "ecall", N, isa_raise_intr(s, ECALL_M));
  INSTPAT("000000000001 00000 000 00000 1110011", "ebreak", N, NEMUTRAP(s->pc, R(reg_a0)));

  // csr
  INSTPAT("0001000 00010 00000 000 00000 1110011", "sret", N, NEMUTRAP(s->pc, R(reg_a0)));
  INSTPAT("0011000 00010 00000 000 00000 1110011", "mret", N, s->dnpc = isa_ret_intr());
  INSTPAT("???????????? ????? 001 ????? 1110011", "csrrw", I, { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = R(rs1); R(rd) = tmp; });
  INSTPAT("???????????? ????? 010 ????? 1110011", "csrrs", I, { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = tmp | R(rs1); R(rd) = tmp; });
  INSTPAT("???????????? ????? 011 ????? 1110011", "csrrc", I, { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = tmp & ~R(rs1); R(rd) = tmp; });
  INSTPAT("???????????? ????? 101 ????? 1110011", "csrrwi", I, { word_t *csr_reg = csr(imm); R(rd) = *csr_reg; *csr_reg = rs1; });
  INSTPAT("???????????? ????? 110 ????? 1110011", "csrrsi", I, { word_t *csr_reg = csr(imm); R(rd) = *csr_reg; *csr_reg = rs1 | *csr_reg; });
  INSTPAT("???????????? ????? 111 ????? 1110011", "csrrci", I, { word_t *csr_reg = csr(imm); R(rd) = *csr_reg; *csr_reg = *csr_reg & ~rs1; });


  // riscv64
  INSTPAT("??????? ????? ????? 110 ????? 0000011", "lwu", I, R(rd) = Mr(s, R(rs1) + imm, 4));
  INSTPAT("??????? ????? ????? 011 ????? 0000011", "ld", I, R(rd) = Mr(s, R(rs1) + imm, 8));
  
  INSTPAT("??????? ????? ????? 000 ????? 0011011", "addiw", I, R(rd) = sign_extend(R(rs1) + imm, 32));
  INSTPAT("0000000 ????? ????? 001 ????? 0011011", "slliw", I, R(rd) = sign_extend(R(rs1) << imm, 32));
  INSTPAT("0000000 ????? ????? 101 ????? 0011011", "srliw", I, R(rd) = sign_extend(R32(rs1) >> imm, 32));
  INSTPAT("0100000 ????? ????? 101 ????? 0011011", "sraiw", I, R(rd) = sign_extend(R32S(rs1) >> (imm & 0b11111), 32));
  
  // S Type
  // riscv32
  INSTPAT("??????? ????? ????? 000 ????? 0100011", "sb", S, Mw(s, R(rs1) + imm, 1, R(rs2)));
  INSTPAT("??????? ????? ????? 001 ????? 0100011", "sh", S, Mw(s, R(rs1) + imm, 2, R(rs2)));
  INSTPAT("??????? ????? ????? 010 ????? 0100011", "sw", S, Mw(s, R(rs1) + imm, 4, R(rs2)));
  // riscv64
  INSTPAT("??????? ????? ????? 011 ????? 0100011", "sd", S, Mw(s, R(rs1) + imm, 8, R(rs2)));


  // B Type
  INSTPAT("??????? ????? ????? 000 ????? 1100011", "beq", B, { if (RS(rs1) == RS(rs2)) { s->dnpc = s->pc + imm; }});
  INSTPAT("??????? ????? ????? 001 ????? 1100011", "bne", B, { if (RS(rs1) != RS(rs2)) { s->dnpc = s->pc + imm; }});
  INSTPAT("??????? ????? ????? 100 ????? 1100011", "blt", B, { if (RS(rs1) < RS(rs2)) { s->dnpc = s->pc + imm; }});
  INSTPAT("??????? ????? ????? 101 ????? 1100011", "bge", B, { if (RS(rs1) >= RS(rs2)) { s->dnpc = s->pc + imm; }});
  INSTPAT("??????? ????? ????? 110 ????? 1100011", "bltu", B, { if (R(rs1) < R(rs2)) { s->dnpc = s->pc + imm; }});
  INSTPAT("??????? ????? ????? 111 ????? 1100011", "bgeu", B, { if (R(rs1) >= R(rs2)) { s->dnpc = s->pc + imm; }});

  // U Type
  INSTPAT("????? ????? ????? ????? ????? 0110111", "lui", U, R(rd) = imm);
  INSTPAT("????? ????? ????? ????? ????? 0010111", "auipc", U, R(rd) = s->pc + imm);

  // J Type
  INSTPAT("????? ????? ????? ????? ????? 1101111", "jal", J, {R(rd) = s->pc + 4; s->dnpc = s->pc + imm; display_ftrace(s, rd, rs1, rs2, imm);});

  INSTPAT("??????? ????? ????? ??? ????? ???????", "inv", N, INV(s->pc));
  INSTPAT_END();
/* clang-format on */

  R(reg_zero) = 0; // reset $zero to 0

  display_inst(s, rd, rs1, rs2, imm);

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}