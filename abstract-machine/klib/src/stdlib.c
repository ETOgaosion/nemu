#include <am.h>
#include <klib-macros.h>
#include <klib.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
char *hbrk = NULL;

int rand(void) {
    // RAND_MAX assumed to be 32767
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
    next = seed;
}

int abs(int x) {
    return (x < 0 ? -x : x);
}

int atoi(const char *nptr) {
    int x = 0;
    while (*nptr == ' ') {
        nptr++;
    }
    while (*nptr >= '0' && *nptr <= '9') {
        x = x * 10 + *nptr - '0';
        nptr++;
    }
    return x;
}

void *malloc(size_t size) {
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
    size = (size_t)ROUNDUP(size, 8);
    if (hbrk == NULL) {
        hbrk = (void *)ROUNDUP(((uint64_t)heap.start + (uint64_t)heap.end) / 2, 8);
    }
    char *old = hbrk;
    hbrk += size;
    Log("old: 0x%lx, hbrk: 0x%lx", old, hbrk);
    for (uint64_t *p = (uint64_t *)old; p != (uint64_t *)hbrk; p++) {
        *p = 0;
    }
    return old;
#endif
    return NULL;
}

void free(void *ptr) {}

#endif
