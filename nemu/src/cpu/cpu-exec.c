/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "monitor/watchpoint.h"
#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
    if (ITRACE_COND) {
        itrace_log_write("%s\n", _this->itrace_logbuf);
    }
#endif
#ifdef CONFIG_FTRACE_COND
    if (FTRACE_COND && _this->ftrace_logbuf[0] != 0) {
        ftrace_log_write("%s\n", _this->ftrace_logbuf);
    }
#endif
    if (g_print_step) {
        IFDEF(CONFIG_ITRACE, puts(_this->itrace_logbuf));
    }
    IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
    bool wp_hit = check_wp();
    if (wp_hit) {
        nemu_state.state = NEMU_STOP;
    }
}

static void exec_once(Decode *s, vaddr_t pc) {
    s->pc = pc;
    s->snpc = pc;
#ifdef CONFIG_ITRACE_COND
    memset(s->itrace_logbuf, 0, sizeof(s->itrace_logbuf));
#endif
#ifdef CONFIG_MTRACE_COND
    memset(s->mtrace_logbuf, 0, sizeof(s->mtrace_logbuf));
#endif
#ifdef CONFIG_DTRACE_COND
    memset(s->dtrace_logbuf, 0, sizeof(s->dtrace_logbuf));
#endif
#ifdef CONFIG_FTRACE_COND
    memset(s->ftrace_logbuf, 0, sizeof(s->ftrace_logbuf));
#endif
#ifdef CONFIG_ETRACE_COND
    memset(s->etrace_logbuf, 0, sizeof(s->etrace_logbuf));
#endif
    isa_exec_once(s);
    cpu.last_pc = cpu.pc;
    cpu.pc = s->dnpc;
}

static void execute(uint64_t n) {
    Decode s;
    s.count = 0;
    for (; n > 0; n--) {
        exec_once(&s, cpu.pc);
        g_nr_guest_inst++;
        trace_and_difftest(&s, cpu.pc);
        if (nemu_state.state != NEMU_RUNNING) {
            break;
        }
        IFDEF(CONFIG_DEVICE, device_update());
        s.count++;
    }
}

static void statistic() {
    IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
    Log("host time spent = " NUMBERIC_FMT " us", g_timer);
    Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
    if (g_timer > 0) {
        Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
    } else {
        Log("Finish running in less than 1 us and can not calculate the simulation frequency");
    }
}

void assert_fail_msg() {
    isa_reg_display();
    statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
    g_print_step = (n < MAX_INST_TO_PRINT);
    switch (nemu_state.state) {
    case NEMU_END:
    case NEMU_ABORT:
        printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
        return;
    default:
        nemu_state.state = NEMU_RUNNING;
    }

    uint64_t timer_start = get_time();

    execute(n);

    uint64_t timer_end = get_time();
    g_timer += timer_end - timer_start;

    switch (nemu_state.state) {
    case NEMU_RUNNING:
        nemu_state.state = NEMU_STOP;
        break;

    case NEMU_END:
    case NEMU_ABORT:
        Log("nemu: %s at pc = " FMT_WORD, (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) : (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))), nemu_state.halt_pc);
        // fall through
    case NEMU_QUIT:
        statistic();
    }
}
