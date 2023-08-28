#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

static Context *(*user_handler)(Event, Context *) = NULL;

enum exceptions {
    INST_MISALIGN,
    INST_ACCESS_FAULT,
    INST_ILLEGAL,
    BREAKPOINT,
    LOAD_MISALIGN,
    LOAD_ACCESS_FAULT,
    STORE_MISALIGN,
    STORE_ACCESS_FAULT,
    ECALL_U,
    ECALL_S,
    ECALL_M = 11,
    INST_PAGE_FAULT,
    LOAD_PAGE_FAULT,
    STORE_PAGE_FAULT = 15
};

Context *__am_irq_handle(Context *c) {
    if (user_handler) {
        Event ev = {0};
        if (c->mcause & ((uint64_t)0x1 << 63)) {
            uint64_t cause = c->mcause & ~((uint64_t)0x1 << 63);
            switch (cause) {
                default:
                    ev.event = EVENT_ERROR;
                break;
            }
        }
        switch (c->mcause) {
            case ECALL_M:
                if (c->gpr[reg_a7] == -1) {
                    ev.event = EVENT_YIELD;
                }
                else {
                    ev.event = EVENT_SYSCALL;
                }
                Log("expc: 0x%lx", c->mepc);
                c->mepc += 4;
                Log("newpc: 0x%lx", c->mepc);
                break;
            default:
                ev.event = EVENT_ERROR;
            break;
        }

        c = user_handler(ev, c);
        assert(c != NULL);
    }

    return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *)) {
    // initialize exception entry
    asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

    // register event handler
    user_handler = handler;

    return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
    return NULL;
}

void yield() {
    asm volatile("li a7, -1; ecall");
}

bool ienabled() {
    return false;
}

void iset(bool enable) {}
