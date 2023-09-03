import subprocess
import re

ARCH = 'riscv64'
inst_path = 'include/generated/inst.h'
inst_raw_path = 'include/generated/inst_raw.h'
decode_operation_path = 'include/generated/decode_operation.h'
decode_pattern = r'[0-1]{7}", "[a-z]+"'
decode_pattern_operation = r', ([A-Z]), (.+)\);'
itrace_pattern = r'instruction: [a-z]+;'

if ARCH == 'riscv64':
    # read inst
    insts = []
    codes = []
    operations = []
    inst_types = []
    inst_opcodes = {}
    with open(inst_raw_path, "r") as f:
        for line in f.readlines():
            if len(line) != 0:
                match_str = re.findall(decode_pattern, line)[0]
                inst = match_str[11: -1]
                insts.append(inst)
                inst_opcodes[inst] = match_str[:7]
                match_str = re.findall(decode_pattern_operation, line)[0]
                inst_types.append(match_str[0])
                operation = match_str[1]
                operation = re.sub(r'\(rd\)', '(*rd)', operation)
                operation = re.sub(r'\(rs1\)', '(*rs1)', operation)
                operation = re.sub(r'\(rs2\)', '(*rs2)', operation)
                operations.append(operation)
                codes.append(line)
    with open(decode_operation_path, "w", encoding="utf-8") as f:
        for inst, inst_type, operation in zip(insts, inst_types, operations):
            s = "void riscv64_" + inst + "(Decode *s, uint32_t inst, int *rd, int *rs1, int *rs2, int64_t *imm) {\n    decode_operand(s, rd, rs1, rs2, imm, TYPE_" + inst_type + ");\n    " + operation + ";\n}\n"
            f.write(s)
    decode = {}
    for inst, code in zip(insts, codes):
        decode[inst] = code

    # read files
    count = {}
    for key in decode:
        count[key] = 0

    files_str = subprocess.run(['find', '..', '-name', 'nemu-itrace-log-*.txt'], stdout=subprocess.PIPE).stdout.decode('utf-8')
    files = files_str.split("\n")[:-1]
    for file in files:
        with open(file, 'r', encoding='UTF-8') as f:
            for line in f.readlines():
                res = re.findall(itrace_pattern, line)[0][13:-1]
                count[res] += 1
    count = dict(sorted(count.items(), key=lambda item: item[1], reverse=True))
    print(count)

    if (next(iter(count.values())) == 0):
        count = {'addi': 14768164, 'ld': 12410850, 'addiw': 9377650, 'lw': 7335609, 'sd': 7110888, 'lbu': 4141110, 'bne': 2781428, 'sw': 2573042, 'sb': 2377406, 'add': 1773451, 'slli': 1609271, 'auipc': 1582861, 'addw': 1369747, 'lui': 1317305, 'jal': 1191932, 'mulw': 1023147, 'beq': 1015762, 'andi': 929747, 'slliw': 913973, 'jalr': 836184, 'and': 747848, 'or': 631599, 'blt': 561938, 'lh': 489098, 'sraiw': 401992, 'srli': 329913, 'subw': 315913, 'bge': 285923, 'srliw': 271718, 'lhu': 267110, 'sh': 219600, 'bltu': 206808, 'lwu': 106959, 'bgeu': 96968, 'sub': 82664, 'srlw': 61027, 'sltu': 60385, 'xor': 53855, 'sltiu': 46647, 'xori': 45042, 'mul': 38724, 'lb': 32224, 'csrrs': 24984, 'sraw': 20433, 'csrrw': 16671, 'sllw': 14790, 'divu': 11476, 'remu': 11091, 'ori': 9173, 'ecall': 8328, 'mret': 8324, 'div': 5783, 'divw': 5002, 'slt': 4424, 'sll': 1885, 'rem': 213, 'divuw': 34, 'srai': 20, 'ebreak': 3, 'remw': 1, 'srl': 0, 'sra': 0, 'mulh': 0, 'mulhsu': 0, 'mulhu': 0, 'remuw': 0, 'slti': 0, 'fence': 0, 'fencei': 0, 'sret': 0, 'csrrc': 0, 'csrrwi': 0, 'csrrsi': 0, 'csrrci': 0}
    
    sorted_inst_opcodes = {}
    for key in count:
        if not inst_opcodes[key] in sorted_inst_opcodes:
            sorted_inst_opcodes[inst_opcodes[key]] = [key]
        else:
            sorted_inst_opcodes[inst_opcodes[key]].append(key)
    
    print(sorted_inst_opcodes)

    with open(inst_path, 'w', encoding="UTF-8") as f:
        for key in count:
            f.write(decode[key])