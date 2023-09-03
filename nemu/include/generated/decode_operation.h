void riscv64_add(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) + R(*rs2);
}
void riscv64_sub(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) - R(*rs2);
}
void riscv64_sll(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) << (R(*rs2) & 0b111111);
}
void riscv64_slt(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) < RS(*rs2);
}
void riscv64_sltu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) < R(*rs2);
}
void riscv64_xor(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) ^ R(*rs2);
}
void riscv64_srl(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) >> (R(*rs2) & 0b111111);
}
void riscv64_sra(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) >> (R(*rs2) & 0b111111);
}
void riscv64_or(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) | R(*rs2);
}
void riscv64_and(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) & R(*rs2);
}
void riscv64_mul(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) * R(*rs2);
}
void riscv64_mulh(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = (word_t)(((__int128_t)R(*rs1) * (__int128_t)R(*rs2)) >> 64);
}
void riscv64_mulhsu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = (word_t)(((__int128_t)R(*rs1) * (__uint128_t)R(*rs2)) >> 64);
}
void riscv64_mulhu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = (word_t)(((__uint128_t)R(*rs1) * (__uint128_t)R(*rs2)) >> 64);
}
void riscv64_div(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) / RS(*rs2);
}
void riscv64_divu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) / R(*rs2);
}
void riscv64_rem(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) % RS(*rs2);
}
void riscv64_remu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) % R(*rs2);
}
void riscv64_addw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) + R(*rs2), 32);
}
void riscv64_subw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) - R(*rs2), 32);
}
void riscv64_sllw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) << (R(*rs2) & 0b111111), 32);
}
void riscv64_srlw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32(*rs1) >> (R(*rs2) & 0b111111), 32);
}
void riscv64_sraw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32S(*rs1) >> (R(*rs2) & 0b111111), 32);
}
void riscv64_mulw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) * R(*rs2), 32);
}
void riscv64_divw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32S(*rs1) / R32S(*rs2), 32);
}
void riscv64_divuw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32(*rs1) / R32(*rs2), 32);
}
void riscv64_remw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32S(*rs1) % R32S(*rs2), 32);
}
void riscv64_remuw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32(*rs1) % R32(*rs2), 32);
}
void riscv64_jalr(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    {s->dnpc = (R(*rs1) + *imm) & ~(uint64_t)0x1; R(*rd) = s->pc + 4; display_ftrace(s, *rd, *rs1, *rs2, *imm);};
}
void riscv64_lb(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(Mr(s, R(*rs1) + *imm, 1), 8);
}
void riscv64_lh(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(Mr(s, R(*rs1) + *imm, 2), 16);
}
void riscv64_lw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(Mr(s, R(*rs1) + *imm, 4), 32);
}
void riscv64_ld(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + *imm, 8);
}
void riscv64_lbu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + *imm, 1);
}
void riscv64_lhu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + *imm, 2);
}
void riscv64_lwu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + *imm, 4);
}
void riscv64_addi(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) + *imm;
}
void riscv64_addiw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R(*rs1) + *imm, 32);
}
void riscv64_slti(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = RS(*rs1) < *imm;
}
void riscv64_sltiu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) < (uint64_t)*imm;
}
void riscv64_xori(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) ^ *imm;
}
void riscv64_ori(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) | *imm;
}
void riscv64_andi(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) & *imm;
}
void riscv64_slli(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rd) = R(*rs1) << (*imm & 0b111111);
}
void riscv64_srli(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) >> (*imm & 0b111111);
}
void riscv64_srai(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = RS(*rs1) >> (*imm & 0b111111);
}
void riscv64_slliw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R(*rs1) << *imm, 32);
}
void riscv64_srliw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R32(*rs1) >> *imm, 32);
}
void riscv64_sraiw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R32S(*rs1) >> (*imm & 0b11111), 32);
}
void riscv64_fence(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    fence_op(*imm, false);
}
void riscv64_fencei(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_N);
    fence_op(0, true);
}
void riscv64_ecall(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_N);
    isa_raise_intr(s, ECALL_M);
}
void riscv64_ebreak(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_N);
    NEMUTRAP(s->pc, R(reg_a0));
}
void riscv64_sret(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_N);
    NEMUTRAP(s->pc, R(reg_a0));
}
void riscv64_mret(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_N);
    s->dnpc = isa_ret_intr();
}
void riscv64_csrrw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(*imm); word_t tmp = *csr_reg; *csr_reg = R(*rs1); R(*rd) = tmp; };
}
void riscv64_csrrs(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(*imm); word_t tmp = *csr_reg; *csr_reg = tmp | R(*rs1); R(*rd) = tmp; };
}
void riscv64_csrrc(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(*imm); word_t tmp = *csr_reg; *csr_reg = tmp & ~R(*rs1); R(*rd) = tmp; };
}
void riscv64_csrrwi(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(*imm); R(*rd) = *csr_reg; *csr_reg = *rs1; };
}
void riscv64_csrrsi(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(*imm); R(*rd) = *csr_reg; *csr_reg = *rs1 | *csr_reg; };
}
void riscv64_csrrci(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(*imm); R(*rd) = *csr_reg; *csr_reg = *csr_reg & ~(*rs1); };
}
void riscv64_sb(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + *imm, 1, R(*rs2));
}
void riscv64_sh(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + *imm, 2, R(*rs2));
}
void riscv64_sw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + *imm, 4, R(*rs2));
}
void riscv64_sd(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + *imm, 8, R(*rs2));
}
void riscv64_beq(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) == RS(*rs2)) { s->dnpc = s->pc + *imm; }};
}
void riscv64_bne(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) != RS(*rs2)) { s->dnpc = s->pc + *imm; }};
}
void riscv64_blt(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) < RS(*rs2)) { s->dnpc = s->pc + *imm; }};
}
void riscv64_bge(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) >= RS(*rs2)) { s->dnpc = s->pc + *imm; }};
}
void riscv64_bltu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_B);
    { if (R(*rs1) < R(*rs2)) { s->dnpc = s->pc + *imm; }};
}
void riscv64_bgeu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_B);
    { if (R(*rs1) >= R(*rs2)) { s->dnpc = s->pc + *imm; }};
}
void riscv64_lui(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_U);
    R(*rd) = *imm;
}
void riscv64_auipc(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_U);
    R(*rd) = s->pc + *imm;
}
void riscv64_jal(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, inst, rd, rs1, rs2, imm, TYPE_J);
    { R(*rd) = s->pc + 4; s->dnpc = s->pc + *imm; display_ftrace(s, *rd, *rs1, *rs2, *imm); };
}
