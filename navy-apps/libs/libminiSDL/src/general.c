#include <NDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

int SDL_Init(uint32_t flags) {
  return NDL_Init(flags);
}

void SDL_Quit() {
  NDL_Quit();
}

char *SDL_GetError() {
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  va_list arglist;
  va_start(arglist, fmt);
  vprintf(fmt, arglist);
  va_end(arglist);
  return -1;
}

int SDL_ShowCursor(int toggle) {
  printf("Not Supported: SDL_ShowCursor\n");
  assert(0);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
}
