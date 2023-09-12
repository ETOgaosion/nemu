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
    uintptr_t mcause, mstatus, mepc, mscratch;
    void *pdir;
};

#define GPR1 gpr[reg_a7] // a7
#define GPR2 gpr[reg_a0]
#define GPR3 gpr[reg_a1]
#define GPR4 gpr[reg_a2]
#define GPRx gpr[reg_a0]

#define MSTATUS_MPP_BITS 0x1800
#define MSTATUS_SUM_BITS 0x40000
#define MSTATUS_MXR_BITS 0x80000

#define VA_MASK ((1lu << 39) - 1)
#define NORMAL_PAGE_SHIFT 12lu
#define NORMAL_PGSIZE (1lu << NORMAL_PAGE_SHIFT)
#define PPN_BITS 9lu

/*
 * PTE format:
 * | XLEN-1  10 | 9             8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
 *       PFN      reserved for SW   D   A   G   U   X   W   R   V
 */

#define _PAGE_ACCESSED_OFFSET 6

#define _PAGE_PRESENT ((uint64_t)1 << 0)
#define _PAGE_READ ((uint64_t)1 << 1)   /* Readable */
#define _PAGE_WRITE ((uint64_t)1 << 2)  /* Writable */
#define _PAGE_EXEC ((uint64_t)1 << 3)   /* Executable */
#define _PAGE_USER ((uint64_t)1 << 4)   /* User */
#define _PAGE_GLOBAL ((uint64_t)1 << 5) /* Global */
#define _PAGE_ACCESSED (1 << 6)         /* Set by hardware on any access */
#define _PAGE_DIRTY ((uint64_t)1 << 7)  /* Set by hardware on any write */
#define _PAGE_SOFT ((uint64_t)1 << 8)   /* Reserved for software */

#define _PAGE_PFN_SHIFT 10lu

#define MAP_KERNEL 1
#define MAP_USER 2

typedef uint64_t PTE;

static inline uint64_t get_pfn(PTE entry) {
    return (entry >> _PAGE_PFN_SHIFT);
}

static inline void set_pfn(PTE *entry, uint64_t pfn) {
    uint64_t temp10 = (*entry) % ((uint64_t)1 << 10);
    *entry = (pfn << 10) | temp10;
}

static inline void set_attribute(PTE *entry, uint64_t bits) {
    uint64_t pfn_temp = (*entry) >> 10;
    *entry = (pfn_temp << 10) | bits;
}

static inline void clear_pgdir(uintptr_t pgdir_addr) {
    PTE *pgaddr = (PTE *)pgdir_addr;
    for (int i = 0; i < 512; i++) {
        pgaddr[i] = 0;
    }
}
#endif
