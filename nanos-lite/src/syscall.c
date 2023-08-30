#include "syscall.h"
#include <common.h>
#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "fs.h"

// #define STRACE
#ifdef STRACE
#define STRACE_MAX_LINE 256
#endif

#ifdef STRACE
char strace_buf[STRACE_MAX_LINE];
#endif

extern uint64_t boot_time_epoch_sec;

struct timeval {
    long    tv_sec;     /* seconds */
    long    tv_usec;    /* microseconds */
};

void do_syscall(Context *c) {
    uintptr_t a[4];
    a[0] = c->GPR1;
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;

#ifdef STRACE
    memset(strace_buf, 0, STRACE_MAX_LINE);
    char *p = strace_buf;
    p += snprintf(p, STRACE_MAX_LINE, "syscall: %ld, args: 0x%lx, 0x%lx, 0x%lx; ", a[0], a[1], a[2], a[3]);
#endif

    switch (a[0]) {
        case SYS_exit:
            halt(a[1]);
            break;
        case SYS_yield:
            yield();
            c->GPR2 = 0;
            break;
        case SYS_open:
            c->GPR2 = fs_open((const char *)a[1], a[2], a[3]);
#ifdef STRACE
            p += snprintf(p, STRACE_MAX_LINE, "open file name: %s, ", (const char *)a[1]);
#endif
            break;
        case SYS_read:
            c->GPR2 = fs_read(a[1], (void *)a[2], a[3]);
#ifdef STRACE
            p += snprintf(p, STRACE_MAX_LINE, "read file name: %s, ", get_file_name(a[1]));
#endif
            break;
        case SYS_write:
            c->GPR2 = fs_write(a[1], (const void *)a[2], a[3]);
#ifdef STRACE
            p += snprintf(p, STRACE_MAX_LINE, "write file name: %s, ", get_file_name(a[1]));
#endif
            break;
        case SYS_close:
            c->GPR2 = fs_close(a[1]);
#ifdef STRACE
            p += snprintf(p, STRACE_MAX_LINE, "close file name: %s, ", get_file_name(a[1]));
#endif
            break;
        case SYS_lseek:
            c->GPR2 = fs_lseek(a[1], a[2], a[3]);
#ifdef STRACE
            p += snprintf(p, STRACE_MAX_LINE, "lseek file name: %s, ", get_file_name(a[1]));
#endif
            break;
        case SYS_brk:
            c->GPR2 = 0;
            break;
        case SYS_gettimeofday: {
            AM_TIMER_UPTIME_T rtc;
            rtc = io_read(AM_TIMER_UPTIME);
            if (a[1]) {
                struct timeval *tv = (struct timeval *)a[1];
                tv->tv_sec = boot_time_epoch_sec + rtc.us / 1000000;
                tv->tv_usec = rtc.us % 1000000;
            }
            c->GPR2 = 0;
        }
            break;
        default:
            panic("Unhandled syscall ID = %d", a[0]);
    }
#ifdef STRACE
    p += snprintf(p, STRACE_MAX_LINE, "return value: %d", c->GPR2);
    printf("%s\n", strace_buf);
#endif
}
