#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void cmd_help(char *args);

static void cmd_echo(char *args) {
  sh_printf("%s", args);
}

static void cmd_exec(char *args) {
  args = strtok(args, "\n");
  char *prog = strtok(args, " ");
  char **argv = (char **)malloc(2 * sizeof(char *));
  argv[0] = prog;
  char *ptr = strtok(NULL, " ");
  int argc = 1;
  while (ptr)
  {
    argv[argc++] = ptr;
    ptr = strtok(NULL, " ");
    argv = (char **)realloc(argv, argc * sizeof(char *));
  }
  argv[argc] = NULL;
  
  execvp(prog, argv);
}

static struct {
    const char *name;
    const char *description;
    void (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"echo", "echo things", cmd_echo},
    {"exec", "execute programs", cmd_exec},
};

#define NR_CMD sizeof(cmd_table) / sizeof(cmd_table[0])

static void cmd_help(char *args) {
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;

    if (arg == NULL) {
        /* no argument given */
        for (i = 0; i < NR_CMD; i++) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    } else {
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(arg, cmd_table[i].name) == 0) {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
}

static void sh_handle_cmd(char *str) {
  char *str_end = str + strlen(str);

  /* extract the first token as the command */
  char *cmd = strtok(str, " ");
  if (cmd == NULL) {
      return;
  }

  /* treat the remaining string as the arguments,
    * which may need further parsing
    */
  char *args = cmd + strlen(cmd) + 1;
  if (args >= str_end) {
      args = NULL;
  }

  int i;
  for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
          cmd_table[i].handler(args);
          break;
      }
  }

  if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd);
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        char *res = (char *)term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
