#include <common.h>
#include <klib.h>

extern void do_syscall(Context *c);

static Context *do_event(Event e, Context *c) {
    switch (e.event) {
        case EVENT_YIELD:
            printf("Yield event!\n");
            break;
        case EVENT_SYSCALL:
            do_syscall(c);
            break;
        default:
            panic("Unhandled event ID = %d", e.event);
            break;
    }

    return c;
}

void init_irq(void) {
    Log("Initializing interrupt/exception handler...");
    cte_init(do_event);
}
