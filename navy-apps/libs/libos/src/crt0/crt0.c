#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = 0;
  char **argv = NULL;
  char **envp = NULL;
  if (args) {
    argc = *(int *)args;
    args++;
    argv = (char **)args;
    args += (argc + 1);
  }
  envp = (char **)args;
  environ = envp;
  
  exit(main(argc, argv, envp));
  assert(0);
}
