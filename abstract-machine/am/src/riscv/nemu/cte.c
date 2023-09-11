#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

static Context *(*user_handler)(Event, Context *) = NULL;

enum interrupts {
    USI,
    SSI,
    MSI = 3,
    UTI,
    STI,
    MTI = 7,
    UEI,
    SEI,
    MEI = 11
};

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

extern void __am_get_cur_as(Context *c);
extern void __am_switch(Context *c);

Context *__am_irq_handle(Context *c) {
    __am_get_cur_as(c);
    if (user_handler) {
        Event ev = {0};
        if (c->mcause & ((uint64_t)0x1 << 63)) {
            uint64_t cause = c->mcause & ~((uint64_t)0x1 << 63);
            switch (cause) {
            case MTI:
            case STI:
                Log("Timer Interrupt");
                break;
            default:
                ev.event = EVENT_ERROR;
                break;
            }
        } else {
            switch (c->mcause) {
            case ECALL_M:
            case ECALL_U:
                if (c->gpr[reg_a7] == -1) {
                    ev.event = EVENT_YIELD;
                } else {
                    ev.event = EVENT_SYSCALL;
                }
                c->mepc += 4;
                break;
            default:
                ev.event = EVENT_ERROR;
                break;
            }
        }
        c = user_handler(ev, c);
        assert(c != NULL);
    }
    __am_switch(c);
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
    Context *ctx = kstack.end - sizeof(Context);
    memset((void *)ctx, 0, sizeof(Context));
    ctx->gpr[reg_a0] = (uintptr_t)arg;
    ctx->gpr[reg_sp] = (uintptr_t)ctx;
    ctx->gpr[reg_tp] = (uintptr_t)ctx;
    Log("kcontext: 0x%lx", (uintptr_t)ctx);
    ctx->mepc = (uintptr_t)entry;
    ctx->mstatus = 0xa00001800;
    ctx->pdir = NULL;
    return ctx;
}

void yield() {
    asm volatile("li a7, -1; ecall");
}

bool ienabled() {
    return false;
}

void iset(bool enable) {}
