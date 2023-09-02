int add(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) + R(*rs2);
}
int sub(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) - R(*rs2);
}
int sll(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) << (R(*rs2) & 0b111111);
}
int slt(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) < RS(*rs2);
}
int sltu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) < R(*rs2);
}
int xor(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) ^ R(*rs2);
}
int srl(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) >> (R(*rs2) & 0b111111);
}
int sra(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) >> (R(*rs2) & 0b111111);
}
int or(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) | R(*rs2);
}
int and(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) & R(*rs2);
}
int mul(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) * R(*rs2);
}
int mulh(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = (word_t)(((__int128_t)R(*rs1) * (__int128_t)R(*rs2)) >> 64);
}
int mulhsu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = (word_t)(((__int128_t)R(*rs1) * (__uint128_t)R(*rs2)) >> 64);
}
int mulhu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = (word_t)(((__uint128_t)R(*rs1) * (__uint128_t)R(*rs2)) >> 64);
}
int div(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) / RS(*rs2);
}
int divu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) / R(*rs2);
}
int rem(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = RS(*rs1) % RS(*rs2);
}
int remu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = R(*rs1) % R(*rs2);
}
int addw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) + R(*rs2), 32);
}
int subw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) - R(*rs2), 32);
}
int sllw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) << (R(*rs2) & 0b111111), 32);
}
int srlw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32(*rs1) >> (R(*rs2) & 0b111111), 32);
}
int sraw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32S(*rs1) >> (R(*rs2) & 0b111111), 32);
}
int mulw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R(*rs1) * R(*rs2), 32);
}
int divw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32S(*rs1) / R32S(*rs2), 32);
}
int divuw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32(*rs1) / R32(*rs2), 32);
}
int remw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32S(*rs1) % R32S(*rs2), 32);
}
int remuw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_R);
    R(*rd) = sign_extend(R32(*rs1) % R32(*rs2), 32);
}
int jalr(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    {s->dnpc = (R(*rs1) + imm) & ~(uint64_t)0x1; R(*rd) = s->pc + 4; display_ftrace(s, *rd, *rs1, *rs2, *imm);};
}
int lb(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(Mr(s, R(*rs1) + imm, 1), 8);
}
int lh(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(Mr(s, R(*rs1) + imm, 2), 16);
}
int lw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(Mr(s, R(*rs1) + imm, 4), 32);
}
int ld(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + imm, 8);
}
int lbu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + imm, 1);
}
int lhu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + imm, 2);
}
int lwu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = Mr(s, R(*rs1) + imm, 4);
}
int addi(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) + imm;
}
int addiw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R(*rs1) + imm, 32);
}
int slti(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = RS(*rs1) < imm;
}
int sltiu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) < (uint64_t)imm;
}
int xori(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) ^ imm;
}
int ori(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) | imm;
}
int andi(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) & imm;
}
int slli(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rd) = R(*rs1) << (imm & 0b111111);
}
int srli(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = R(*rs1) >> (imm & 0b111111);
}
int srai(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = RS(*rs1) >> (imm & 0b111111);
}
int slliw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R(*rs1) << imm, 32);
}
int srliw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R32(*rs1) >> imm, 32);
}
int sraiw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    R(*rd) = sign_extend(R32S(*rs1) >> (imm & 0b11111), 32);
}
int fence(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    fence_op(imm, false);
}
int fencei(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_N);
    fence_op(0, true);
}
int ecall(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_N);
    isa_raise_intr(s, ECALL_M);
}
int ebreak(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_N);
    NEMUTRAP(s->pc, R(reg_a0));
}
int sret(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_N);
    NEMUTRAP(s->pc, R(reg_a0));
}
int mret(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_N);
    s->dnpc = isa_ret_intr();
}
int csrrw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = R(*rs1); R(*rd) = tmp; };
}
int csrrs(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = tmp | R(*rs1); R(*rd) = tmp; };
}
int csrrc(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = tmp & ~R(*rs1); R(*rd) = tmp; };
}
int csrrwi(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(imm); R(*rd) = *csr_reg; *csr_reg = rs1; };
}
int csrrsi(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(imm); R(*rd) = *csr_reg; *csr_reg = rs1 | *csr_reg; };
}
int csrrci(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_I);
    { word_t *csr_reg = csr(imm); R(*rd) = *csr_reg; *csr_reg = *csr_reg & ~rs1; };
}
int sb(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + imm, 1, R(*rs2));
}
int sh(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + imm, 2, R(*rs2));
}
int sw(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + imm, 4, R(*rs2));
}
int sd(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_S);
    Mw(s, R(*rs1) + imm, 8, R(*rs2));
}
int beq(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) == RS(*rs2)) { s->dnpc = s->pc + imm; }};
}
int bne(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) != RS(*rs2)) { s->dnpc = s->pc + imm; }};
}
int blt(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) < RS(*rs2)) { s->dnpc = s->pc + imm; }};
}
int bge(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_B);
    { if (RS(*rs1) >= RS(*rs2)) { s->dnpc = s->pc + imm; }};
}
int bltu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_B);
    { if (R(*rs1) < R(*rs2)) { s->dnpc = s->pc + imm; }};
}
int bgeu(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_B);
    { if (R(*rs1) >= R(*rs2)) { s->dnpc = s->pc + imm; }};
}
int lui(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_U);
    R(*rd) = imm;
}
int auipc(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_U);
    R(*rd) = s->pc + imm;
}
int jal(Decode *s, int *rd, int *rs1, int *rs2, int64_t *imm) {
    decode_operand(s, rd, rs1, rs2, imm, TYPE_J);
    { R(*rd) = s->pc + 4; s->dnpc = s->pc + imm; display_ftrace(s, *rd, *rs1, *rs2, *imm); };
}
