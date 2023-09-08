#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

#ifndef PGSIZE
#define PGSIZE 4096
#endif
#define ROUND(a, n) (((((uint64_t)(a)) + (n)-1)) & ~((n)-1))

#define PG_ALIGN __attribute((aligned(PGSIZE)))

void *new_page(size_t);

#endif
