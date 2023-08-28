#include <common.h>
#include <klib.h>

static Context *do_event(Event e, Context *c) {
    switch (e.event) {
        case EVENT_YIELD:
            printf("Yield event!\n");
            break;
        case EVENT_SYSCALL:
            printf("Syscall event! pc: 0x%lx\n", c->mepc);
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
