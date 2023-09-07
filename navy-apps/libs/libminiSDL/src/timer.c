#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>
#include <assert.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  printf("Not Supported: SDL_AddTimer\n");
  assert(0);
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  printf("Not Supported: SDL_RemoveTimer\n");
  assert(0);
  return 1;
}

uint32_t SDL_GetTicks() {
  return NDL_GetTicks();
}

void SDL_Delay(uint32_t ms) {
  int time = NDL_GetTicks();
  while (NDL_GetTicks() - time < ms) {
    ;
  }
}
