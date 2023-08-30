import subprocess

ARCH = "riscv64"

if ARCH == "riscv64":
    inst = {
        "add": "INSTPAT(\"0000000 ????? ????? 000 ????? 0110011\", \"add\", R, R(rd) = R(rs1) + R(rs2));",
        "sub": "INSTPAT(\"0100000 ????? ????? 000 ????? 0110011\", \"sub\", R, R(rd) = R(rs1) - R(rs2));",
        "sll": "INSTPAT(\"0000000 ????? ????? 001 ????? 0110011\", \"sll\", R, R(rd) = R(rs1) << (R(rs2) & 0b111111));",
        "slt": "INSTPAT(\"0000000 ????? ????? 010 ????? 0110011\", \"slt\", R, R(rd) = RS(rs1) < RS(rs2));",
        "sltu": "INSTPAT(\"0000000 ????? ????? 011 ????? 0110011\", \"sltu\", R, R(rd) = R(rs1) < R(rs2));",
        "xor": "INSTPAT(\"0000000 ????? ????? 100 ????? 0110011\", \"xor\", R, R(rd) = R(rs1) ^ R(rs2));",
        "srl": "INSTPAT(\"0000000 ????? ????? 101 ????? 0110011\", \"srl\", R, R(rd) = R(rs1) >> (R(rs2) & 0b111111));",
        "sra": "INSTPAT(\"0100000 ????? ????? 101 ????? 0110011\", \"sra\", R, R(rd) = RS(rs1) >> (R(rs2) & 0b111111));",
        "or": "INSTPAT(\"0000000 ????? ????? 110 ????? 0110011\", \"or\", R, R(rd) = R(rs1) | R(rs2));",
        "and": "INSTPAT(\"0000000 ????? ????? 111 ????? 0110011\", \"and\", R, R(rd) = R(rs1) & R(rs2));",
        # riscv32m
        "mul": "INSTPAT(\"0000001 ????? ????? 000 ????? 0110011\", \"mul\", R, R(rd) = R(rs1) * R(rs2));",
        "mulh": "INSTPAT(\"0000001 ????? ????? 001 ????? 0110011\", \"mulh\", R, R(rd) = (word_t)(((__int128_t)R(rs1) * (__int128_t)R(rs2)) >> 64));",
        "mulhsu": "INSTPAT(\"0000001 ????? ????? 010 ????? 0110011\", \"mulhsu\", R, R(rd) = (word_t)(((__int128_t)R(rs1) * (__uint128_t)R(rs2)) >> 64));",
        "mulhu": "INSTPAT(\"0000001 ????? ????? 011 ????? 0110011\", \"mulhu\", R, R(rd) = (word_t)(((__uint128_t)R(rs1) * (__uint128_t)R(rs2)) >> 64));",
        "div": "INSTPAT(\"0000001 ????? ????? 100 ????? 0110011\", \"div\", R, R(rd) = RS(rs1) / RS(rs2));",
        "divu": "INSTPAT(\"0000001 ????? ????? 101 ????? 0110011\", \"divu\", R, R(rd) = R(rs1) / R(rs2));",
        "rem": "INSTPAT(\"0000001 ????? ????? 110 ????? 0110011\", \"rem\", R, R(rd) = RS(rs1) % RS(rs2));",
        "remu": "INSTPAT(\"0000001 ????? ????? 111 ????? 0110011\", \"remu\", R, R(rd) = R(rs1) % R(rs2));",

        # riscv64
        "addw": "INSTPAT(\"0000000 ????? ????? 000 ????? 0111011\", \"addw\", R, R(rd) = sign_extend(R(rs1) + R(rs2), 32));",
        "subw": "INSTPAT(\"0100000 ????? ????? 000 ????? 0111011\", \"subw\", R, R(rd) = sign_extend(R(rs1) - R(rs2), 32));",
        "sllw": "INSTPAT(\"0000000 ????? ????? 001 ????? 0111011\", \"sllw\", R, R(rd) = sign_extend(R(rs1) << (R(rs2) & 0b111111), 32));",
        "srlw": "INSTPAT(\"0000000 ????? ????? 101 ????? 0111011\", \"srlw\", R, R(rd) = sign_extend(R32(rs1) >> (R(rs2) & 0b111111), 32));",
        "sraw": "INSTPAT(\"0100000 ????? ????? 101 ????? 0111011\", \"sraw\", R, R(rd) = sign_extend(R32S(rs1) >> (R(rs2) & 0b111111), 32));",
        # riscv64m
        "mulw": "INSTPAT(\"0000001 ????? ????? 000 ????? 0111011\", \"mulw\", R, R(rd) = sign_extend(R(rs1) * R(rs2), 32));",
        "divw": "INSTPAT(\"0000001 ????? ????? 100 ????? 0111011\", \"divw\", R, R(rd) = sign_extend(R32S(rs1) / R32S(rs2), 32));",
        "divuw": "INSTPAT(\"0000001 ????? ????? 101 ????? 0111011\", \"divuw\", R, R(rd) = sign_extend(R32(rs1) / R32(rs2), 32));",
        "remw": "INSTPAT(\"0000001 ????? ????? 110 ????? 0111011\", \"remw\", R, R(rd) = sign_extend(R32S(rs1) % R32S(rs2), 32));",
        "remuw": "INSTPAT(\"0000001 ????? ????? 111 ????? 0111011\", \"remuw\", R, R(rd) = sign_extend(R32(rs1) % R32(rs2), 32));",

        # I Type
        # riscv32
        "jalr": "INSTPAT(\"??????? ????? ????? 000 ????? 1100111\", \"jalr\", I, {s->dnpc = (R(rs1) + imm) & ~(uint64_t)0x1; R(rd) = s->pc + 4; display_ftrace(s, rd, rs1, rs2, imm);});",

        "lb": "INSTPAT(\"??????? ????? ????? 000 ????? 0000011\", \"lb\", I, R(rd) = sign_extend(Mr(s, R(rs1) + imm, 1), 8));",
        "lh": "INSTPAT(\"??????? ????? ????? 001 ????? 0000011\", \"lh\", I, R(rd) = sign_extend(Mr(s, R(rs1) + imm, 2), 16));",
        "lw": "INSTPAT(\"??????? ????? ????? 010 ????? 0000011\", \"lw\", I, R(rd) = sign_extend(Mr(s, R(rs1) + imm, 4), 32));",
        "lbu": "INSTPAT(\"??????? ????? ????? 100 ????? 0000011\", \"lbu\", I, R(rd) = Mr(s, R(rs1) + imm, 1));",
        "lhu": "INSTPAT(\"??????? ????? ????? 101 ????? 0000011\", \"lhu\", I, R(rd) = Mr(s, R(rs1) + imm, 2));",

        "addi": "INSTPAT(\"??????? ????? ????? 000 ????? 0010011\", \"addi\", I, R(rd) = R(rs1) + imm);",
        "slti": "INSTPAT(\"??????? ????? ????? 010 ????? 0010011\", \"slti\", I, R(rd) = RS(rs1) < imm);",
        "sltiu": "INSTPAT(\"??????? ????? ????? 011 ????? 0010011\", \"sltiu\", I, R(rd) = R(rs1) < (uint64_t)imm);",
        "xori": "INSTPAT(\"??????? ????? ????? 100 ????? 0010011\", \"xori\", I, R(rd) = R(rs1) ^ imm);",
        "ori": "INSTPAT(\"??????? ????? ????? 110 ????? 0010011\", \"ori\", I, R(rd) = R(rs1) | imm);",
        "andi": "INSTPAT(\"??????? ????? ????? 111 ????? 0010011\", \"andi\", I, R(rd) = R(rs1) & imm);",

        # WTF? riscv64 and riscv32 have different formats defined, but not shown in riscvg --!
        "slli": "INSTPAT(\"000000 ?????? ????? 001 ????? 0010011\", \"slli\", I, R(rd) = R(rd) = R(rs1) << (imm & 0b111111));",
        "srli": "INSTPAT(\"000000 ?????? ????? 101 ????? 0010011\", \"srli\", I, R(rd) = R(rs1) >> (imm & 0b111111));",
        "srai": "INSTPAT(\"010000 ?????? ????? 101 ????? 0010011\", \"srai\", I, R(rd) = RS(rs1) >> (imm & 0b111111));",

        "fence": "INSTPAT(\"0000 ???? ???? 00000 000 00000 0001111\", \"fence\", I, fence_op(imm, false));",
        "fence_I": "INSTPAT(\"0000 0000 0000 00000 001 00000 0001111\", \"fence_I\", N, fence_op(0, true));",

        "ecall": "INSTPAT(\"000000000000 00000 000 00000 1110011\", \"ecall\", N, isa_raise_intr(s, ECALL_M));",
        "ebreak": "INSTPAT(\"000000000001 00000 000 00000 1110011\", \"ebreak\", N, NEMUTRAP(s->pc, R(reg_a0)));",

        # csr
        "sret": "INSTPAT(\"0001000 00010 00000 000 00000 1110011\", \"sret\", N, NEMUTRAP(s->pc, R(reg_a0)));",
        "mret": "INSTPAT(\"0011000 00010 00000 000 00000 1110011\", \"mret\", N, s->dnpc = isa_ret_intr());",
        "csrrw": "INSTPAT(\"???????????? ????? 001 ????? 1110011\", \"csrrw\", I, { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = R(rs1); R(rd) = tmp; });",
        "csrrs": "INSTPAT(\"???????????? ????? 010 ????? 1110011\", \"csrrs\", I, { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = tmp | R(rs1); R(rd) = tmp; });",
        "csrrc": "INSTPAT(\"???????????? ????? 011 ????? 1110011\", \"csrrc\", I, { word_t *csr_reg = csr(imm); word_t tmp = *csr_reg; *csr_reg = tmp & ~R(rs1); R(rd) = tmp; });",
        "csrrwi": "INSTPAT(\"???????????? ????? 101 ????? 1110011\", \"csrrwi\", I, { word_t *csr_reg = csr(imm); R(rd) = *csr_reg; *csr_reg = rs1; });",
        "csrrsi": "INSTPAT(\"???????????? ????? 110 ????? 1110011\", \"csrrsi\", I, { word_t *csr_reg = csr(imm); R(rd) = *csr_reg; *csr_reg = rs1 | *csr_reg; });",
        "csrrci": "INSTPAT(\"???????????? ????? 111 ????? 1110011\", \"csrrci\", I, { word_t *csr_reg = csr(imm); R(rd) = *csr_reg; *csr_reg = *csr_reg & ~rs1; });",


        # riscv64
        "lwu": "INSTPAT(\"??????? ????? ????? 110 ????? 0000011\", \"lwu\", I, R(rd) = Mr(s, R(rs1) + imm, 4));",
        "ld": "INSTPAT(\"??????? ????? ????? 011 ????? 0000011\", \"ld\", I, R(rd) = Mr(s, R(rs1) + imm, 8));",
  
        "addiw": "INSTPAT(\"??????? ????? ????? 000 ????? 0011011\", \"addiw\", I, R(rd) = sign_extend(R(rs1) + imm, 32));",
        "slliw": "INSTPAT(\"0000000 ????? ????? 001 ????? 0011011\", \"slliw\", I, R(rd) = sign_extend(R(rs1) << imm, 32));",
        "srliw": "INSTPAT(\"0000000 ????? ????? 101 ????? 0011011\", \"srliw\", I, R(rd) = sign_extend(R32(rs1) >> imm, 32));",
        "sraiw": "INSTPAT(\"0100000 ????? ????? 101 ????? 0011011\", \"sraiw\", I, R(rd) = sign_extend(R32S(rs1) >> (imm & 0b11111), 32));",
  
        # S Type
        # riscv32
        "sb": "INSTPAT(\"??????? ????? ????? 000 ????? 0100011\", \"sb\", S, Mw(s, R(rs1) + imm, 1, R(rs2)));",
        "sh": "INSTPAT(\"??????? ????? ????? 001 ????? 0100011\", \"sh\", S, Mw(s, R(rs1) + imm, 2, R(rs2)));",
        "sw": "INSTPAT(\"??????? ????? ????? 010 ????? 0100011\", \"sw\", S, Mw(s, R(rs1) + imm, 4, R(rs2)));",
        # riscv64
        "sd": "INSTPAT(\"??????? ????? ????? 011 ????? 0100011\", \"sd\", S, Mw(s, R(rs1) + imm, 8, R(rs2)));",


        # B Type
        "beq": "INSTPAT(\"??????? ????? ????? 000 ????? 1100011\", \"beq\", B, { if (RS(rs1) == RS(rs2)) { s->dnpc = s->pc + imm; }});",
        "bne": "INSTPAT(\"??????? ????? ????? 001 ????? 1100011\", \"bne\", B, { if (RS(rs1) != RS(rs2)) { s->dnpc = s->pc + imm; }});",
        "blt": "INSTPAT(\"??????? ????? ????? 100 ????? 1100011\", \"blt\", B, { if (RS(rs1) < RS(rs2)) { s->dnpc = s->pc + imm; }});",
        "bge": "INSTPAT(\"??????? ????? ????? 101 ????? 1100011\", \"bge\", B, { if (RS(rs1) >= RS(rs2)) { s->dnpc = s->pc + imm; }});",
        "bltu": "INSTPAT(\"??????? ????? ????? 110 ????? 1100011\", \"bltu\", B, { if (R(rs1) < R(rs2)) { s->dnpc = s->pc + imm; }});",
        "bgeu": "INSTPAT(\"??????? ????? ????? 111 ????? 1100011\", \"bgeu\", B, { if (R(rs1) >= R(rs2)) { s->dnpc = s->pc + imm; }});",

        # U Type
        "lui": "INSTPAT(\"????? ????? ????? ????? ????? 0110111\", \"lui\", U, R(rd) = imm);",
        "auipc": "INSTPAT(\"????? ????? ????? ????? ????? 0010111\", \"auipc\", U, R(rd) = s->pc + imm);",

        # J Type
        "jal": "INSTPAT(\"????? ????? ????? ????? ????? 1101111\", \"jal\", J, {R(rd) = s->pc + 4; s->dnpc = s->pc + imm; display_ftrace(s, rd, rs1, rs2, imm);});",
    }
    count = {}
    for key in inst:
        count[key] = 0
    p = subprocess.run("find", "..", "-name", "")
    