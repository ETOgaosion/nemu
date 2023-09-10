#include <am.h>
#include <memory.h>

static void *pf = NULL;
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
    return 0;
}

void init_mm() {
    pf = (void *)ROUNDUP(heap.start, PGSIZE);
    Log("free physical pages starting from 0x%x", pf);

#ifdef HAS_VME
    vme_init(pg_alloc, free_page);
#endif
}
