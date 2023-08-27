#ifndef ARCH_H__
#define ARCH_H__

#define reg_zero 0
#define reg_ra 1
#define reg_sp 2
#define reg_gp 3
#define reg_tp 4
#define reg_t0 5
#define reg_t1 6
#define reg_t2 7
#define reg_s0 8
#define reg_s1 9
#define reg_a0 10
#define reg_a1 11
#define reg_a2 12
#define reg_a3 13
#define reg_a4 14
#define reg_a5 15
#define reg_a6 16
#define reg_a7 17
#define reg_s2 18
#define reg_s3 19
#define reg_s4 20
#define reg_s5 21
#define reg_s6 22
#define reg_s7 23
#define reg_s8 24
#define reg_s9 25
#define reg_s10 26
#define reg_s11 27
#define reg_t3 28
#define reg_t4 29
#define reg_t5 30
#define reg_t6 31

struct Context {
    // TODO: fix the order of these members to match trap.S
    uintptr_t gpr[32];
    uintptr_t mcause, mstatus, mepc;
    void *pdir;
};

#define GPR1 gpr[reg_a7] // a7
#define GPR2 gpr[reg_a0]
#define GPR3 gpr[reg_a1]
#define GPR4 gpr[reg_a2]
#define GPRx gpr[reg_a3]
#endif
