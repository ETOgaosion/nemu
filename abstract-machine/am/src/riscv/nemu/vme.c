#include <am.h>
#include <klib.h>
#include <nemu.h>

static AddrSpace kas = {};
static void *(*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void *) = NULL;
static int vme_enable = 0;

static Area segments[] = { // Kernel memory mappings
    NEMU_PADDR_SPACE};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
    uintptr_t mode = 1ul << (__riscv_xlen - 1);
    asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
    uintptr_t satp;
    asm volatile("csrr %0, satp" : "=r"(satp));
    return satp << 12;
}

bool vme_init(void *(*pgalloc_f)(int), void (*pgfree_f)(void *)) {
    pgalloc_usr = pgalloc_f;
    pgfree_usr = pgfree_f;

    kas.ptr = pgalloc_f(PGSIZE);

    int i;
    for (i = 0; i < LENGTH(segments); i++) {
        void *va = segments[i].start;
        for (; va < segments[i].end; va += PGSIZE) {
            map(&kas, va, va, 0);
        }
    }
    set_satp(kas.ptr);
    vme_enable = 1;

    return true;
}

void protect(AddrSpace *as) {
    PTE *updir = (PTE *)(pgalloc_usr(PGSIZE));
    as->ptr = updir;
    as->area = USER_SPACE;
    as->pgsize = PGSIZE;
    // map kernel space
    memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {}

void __am_get_cur_as(Context *c) {
    c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
    if (vme_enable && c->pdir != NULL) {
        set_satp(c->pdir);
    }
}

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

static inline PTE check_page_set_flag(PTE *page, uint64_t vpn, uint64_t flag) {
    if (page[vpn] & _PAGE_PRESENT) {
        return (get_pfn(page[vpn]) << NORMAL_PAGE_SHIFT);
    } else {
        uint64_t newpage = (uint64_t)pgalloc_usr(PGSIZE);
        set_pfn(&page[vpn], newpage >> NORMAL_PAGE_SHIFT);
        set_attribute(&page[vpn], flag);
        return newpage;
    }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
    uint64_t vpn[] = {
        ((uint64_t)va >> 12) & ((0x1lu << 9) - 1), // vpn0
        ((uint64_t)va >> 21) & ((0x1lu << 9) - 1), // vpn1
        ((uint64_t)va >> 30) & ((0x1lu << 9) - 1)  // vpn2
    };
    /* the PTE in the first page_table */
    PTE *page_base = (uint64_t *)as->ptr;
    /* second page */
    PTE *second_page = NULL;
    /* finally page */
    PTE *third_page = NULL;
    /* find the second page */
    second_page = (PTE *)check_page_set_flag(page_base, vpn[2], _PAGE_PRESENT);
    /* third page */
    third_page = (PTE *)check_page_set_flag(second_page, vpn[1], _PAGE_PRESENT);
    /* the physical page */
    set_pfn(&third_page[vpn[0]], (uint64_t)pa >> NORMAL_PAGE_SHIFT);
    /* maybe need to assign U to low */
    // Generate flags
    /* the R,W,X == 1 will be the leaf */
    uint64_t pte_flags = _PAGE_PRESENT | (uint64_t)prot;
    set_attribute(&third_page[vpn[0]], pte_flags);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
    Context *ctx = kstack.end - sizeof(Context);
    memset((void *)ctx, 0, sizeof(Context));
    ctx->gpr[reg_sp] = (uintptr_t)ctx;
    ctx->mepc = (uintptr_t)entry;
    ctx->mstatus = 0xa00001800;
    return ctx;
}
