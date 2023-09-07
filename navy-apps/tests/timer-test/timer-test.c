#include <syscall.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#define USE_NDL

#ifndef USE_NDL
#include "syscall.h"

int main() {
    time_t boot_sec = 0;
    struct timeval tv;
    struct timeval last_tv = {0};
    _gettimeofday(&tv, NULL);
    boot_sec = tv.tv_sec;
    while (1)
    {
        _gettimeofday(&tv, NULL);
        tv.tv_sec -= boot_sec;
        if (tv.tv_sec * 10 + tv.tv_usec / 100000 - (last_tv.tv_sec * 10 + last_tv.tv_usec / 100000) > 5) {
            printf("now time: sec: %ld, usec: %ld\n", tv.tv_sec, tv.tv_usec);
            memcpy((void *)&last_tv, (const void *)&tv, sizeof(struct timeval));
        }
    }
    return 0;
}
#else
#include <NDL.h>

int main() {
    time_t msec = 0;
    time_t last_msec = 0;
    NDL_Init(0);
    while (1)
    {
        msec = NDL_GetTicks();
        if (msec - last_msec > 500) {
            printf("now time: msec: %ld\n", msec);
            last_msec = msec;
        }
    }
    return 0;
}
#endif