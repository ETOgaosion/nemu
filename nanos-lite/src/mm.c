#include <am.h>
#include <arch/riscv64-nemu.h>
#include <klib-macros.h>
#include <memory.h>
#include <proc.h>

extern void *pf;
extern char *hbrk;

void *new_page(size_t nr_page) {
    void *old = pf;
    memset(pf, 0, nr_page * PGSIZE);
    pf += nr_page * PGSIZE;
    return old;
}

#ifdef HAS_VME
static void *pg_alloc(int n) {
    return new_page(n / PGSIZE);
}
#endif

void free_page(void *p) {
    panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
    if (brk > current->max_brk) {
        int page_num = (brk - current->max_brk + PGSIZE - 1) / PGSIZE;
        void *pg = new_page(page_num);
        for (int i = 0; i < page_num; i++) {
            map(&current->as, (void *)(current->max_brk + i * PGSIZE), pg + i * PGSIZE, _PAGE_WRITE | _PAGE_READ | _PAGE_USER);
        }
        current->max_brk += page_num * PGSIZE;
    }
    return 0;
}

void init_mm() {
    pf = (void *)ROUNDUP(heap.start, PGSIZE);
    Log("free physical pages starting from 0x%x", pf);

#ifdef HAS_VME
    vme_init(pg_alloc, free_page);
#endif
}
