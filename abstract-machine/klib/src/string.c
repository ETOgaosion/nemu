#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define UCHAR_MAX (0xffU)

#define SS (sizeof(size_t))
#define ALIGN (sizeof(size_t) - 1)
#define ONES ((size_t)-1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)

size_t strlen(const char *src) {
    int i;
    for (i = 0; src[i] != '\0'; i++) {}
    return i;
}

char *strcpy(char *dest, const char *src) {
    char *tmp = dest;

    while (*src) {
        *dest++ = *src++;
    }

    *dest = '\0';

    return tmp;
}

char *strncpy(char *d, const char *s, size_t n) {
    typedef size_t __attribute__((__may_alias__)) word;
    word *wd;
    const word *ws;
    if (((uintptr_t)s & ALIGN) == ((uintptr_t)d & ALIGN)) {
        for (; ((uintptr_t)s & ALIGN) && n && (*d = *s); n--, s++, d++)
            ;
        if (!n || !*s) {
            goto tail;
        }
        wd = (void *)d;
        ws = (const void *)s;
        for (; n >= sizeof(size_t) && !HASZERO(*ws); n -= sizeof(size_t), ws++, wd++) {
            *wd = *ws;
        }
        d = (void *)wd;
        s = (const void *)ws;
    }
    for (; n && (*d = *s); n--, s++, d++)
        ;
tail:
    memset(d, 0, n);
    return d;
}

char *strcat(char *dest, const char *src) {
    char *tmp = dest;

    while (*dest != '\0') {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }

    *dest = '\0';

    return tmp;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return (*str1) - (*str2);
        }
        ++str1;
        ++str2;
    }
    return (*str1) - (*str2);
}

int strncmp(const char *_l, const char *_r, size_t n) {
    const unsigned char *l = (void *)_l, *r = (void *)_r;
    if (!n--) {
        return 0;
    }
    for (; *l && *r && n && *l == *r; l++, r++, n--)
        ;
    return *l - *r;
}

void *memset(void *dest, int val, size_t len) {
    uint8_t *dst = (uint8_t *)dest;

    for (; len != 0; len--) {
        *dst++ = val;
    }

    return dest;
}

void *memmove(void *dst, const void *src, size_t n) {
    char p_tmp[100];
    memcpy(p_tmp, src, n);
    memcpy(dst, p_tmp, n);
    return dst;
}

void *memcpy(void *dest, const void *src, size_t len) {
    uint8_t *dst = (uint8_t *)dest;

    for (; len != 0; len--) {
        *(uint8_t *)dst++ = *(const uint8_t *)src++;
    }
    return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
    for (int i = 0; i < num; ++i) {
        if (((char *)ptr1)[i] != ((char *)ptr2)[i]) {
            return ((char *)ptr1)[i] - ((char *)ptr2)[i];
        }
    }
    return 0;
}

#endif
