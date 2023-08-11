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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "monitor/expr.h"
#include "monitor/sdb.h"
#include "monitor/watchpoint.h"
#include "memory/paddr.h"
#include "utils.h"

static int is_batch_mode = __BATCH_MODE__;

void init_regex();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_END;
  return -1;
}

static int cmd_si(char *args) {
  int steps = 1;
  if (args) {
    steps = atoi((const char *)args);
    if (steps < 1) {
      steps = 1;
    }
  }
  cpu_exec(steps);
  return steps;
}

static int cmd_i(char *args) {
  if (args[0] == 'r') {
    isa_reg_display();
  }
  else if (args[0] == 'w') {
    display_wp_pool();
  }
  return 0;
}


static int cmd_x(char *args) {
  char *length_str = strtok(args, " ");
  int length = atoi(length_str);
  args += strlen(args) + 1;
  bool success = false;
  word_t val = expr(args, &success), addr_base = val;
  for (int i = 0; i < length / 4; i++) {
    printf("[0x%16lx]: 0x%16lx 0x%16lx 0x%16lx 0x%16lx\n", addr_base, paddr_read(addr_base, 4), paddr_read(addr_base + 1, 4), paddr_read(addr_base + 2, 4), paddr_read(addr_base + 3, 4));
    addr_base += 16;
  }
  if (length % 4) {
    printf("[0x%16lx]:", addr_base);
  }
  for (int i = 0; i < length % 4; i++) {
    printf(" 0x%16lx", paddr_read(addr_base + i, 4));
  }
  printf("\n");
  return val;
}

static int cmd_p(char *args) {
  bool success = false;
  word_t val = expr(args, &success);
  printf("0x%16lx\n", val);
  return val;
}

static int cmd_w(char *args) {
  int num = 0;
  WP *watchpoint = alloc_wp(&num, args);
  printf("add watchpoint %d", watchpoint->NO);
  return num;
}

static int cmd_d(char *args) {
  int num = atoi(args);
  free_wp(num);
  return num;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more args */
  { "si", "Step into a function call(step instruction)", cmd_si},
  { "info", "Information of registers and watch points", cmd_i},
  { "x", "Scan memory", cmd_x},
  { "p", "Print value of expr", cmd_p},
  { "w", "Set watch points", cmd_w},
  { "d", "Delete watchpoints", cmd_d},

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  // test_expr();

  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
