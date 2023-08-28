#include "syscall.h"
#include <common.h>
#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define STRACE

void do_syscall(Context *c) {
    uintptr_t a[4];
    a[0] = c->GPR1;
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;

#ifdef STRACE
    printf("syscall: %ld, args: 0x%lx, 0x%lx, 0x%lx\n", a[0], a[1], a[2], a[3]);
#endif

    switch (a[0]) {
        case SYS_exit:
            halt(a[1]);
            break;
        case SYS_yield:
            yield();
            c->GPR2 = 0;
            break;
        case SYS_write:
            if (a[1] == 1 || a[1] == 2) {
                char *buf = (char *)a[2];
                for (int i = 0; i < a[3]; i++) {
                    putch(buf[i]);
                }
                c->GPR2 = a[3];
            }
            break;
        case SYS_brk:
            c->GPR2 = 0;
            break;
        default:
            panic("Unhandled syscall ID = %d", a[0]);
    }
}
