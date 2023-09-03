static inline void handle_add_sub_mul(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_6) {
        return riscv64_sub(s, inst, rd, rs1, rs2, imm);
    }
    else if (inst & FRONTCODE7_1) {
        return riscv64_mul(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_add(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_sll_mulh(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_1) {
        return riscv64_mulh(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_sll(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_slt_mulhsu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_1) {
        return riscv64_mulhsu(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_slt(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_sltu_mulhu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_1) {
        return riscv64_mulhu(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_sltu(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_xor_div(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_1) {
        return riscv64_div(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_xor(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_srl_sra_divu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_6) {
        return riscv64_sra(s, inst, rd, rs1, rs2, imm);
    }
    else if (inst & FRONTCODE7_1) {
        return riscv64_divu(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_srl(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_or_rem(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_1) {
        return riscv64_rem(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_or(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_and_remu(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_1) {
        return riscv64_remu(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_and(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_addw_subw_mulw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_6) {
        return riscv64_subw(s, inst, rd, rs1, rs2, imm);
    }
    else if (inst & FRONTCODE7_1) {
        return riscv64_mulw(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_addw(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_srlw_sraw_divuw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_6) {
        return riscv64_sraw(s, inst, rd, rs1, rs2, imm);
    }
    else if (inst & FRONTCODE7_1) {
        return riscv64_divuw(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_srlw(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_srli_srai(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_6) {
        return riscv64_srai(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_srli(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_srliw_sraiw(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE7_6) {
        return riscv64_sraiw(s, inst, rd, rs1, rs2, imm);
    }
    else {
        return riscv64_srliw(s, inst, rd, rs1, rs2, imm);
    }
}
static inline void handle_ecall_ebreak_sret_mret(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {
    if (inst & FRONTCODE12_1) {
        return riscv64_ebreak(s, inst, rd, rs1, rs2, imm);
    }
    else if (inst & FRONTCODE12_2) {
        if (inst & FRONTCODE12_10) {
            return riscv64_mret(s, inst, rd, rs1, rs2, imm);
        }
        else {
            return riscv64_sret(s, inst, rd, rs1, rs2, imm);
        }
    }
    else {
        return riscv64_ecall(s, inst, rd, rs1, rs2, imm);
    }
}