#include <NDL.h>
#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

#define keyboard_length (sizeof(keyname)) / (sizeof(const char *))
static uint8_t keystate[keyboard_length] = {0};

static int keyname_size = sizeof(keyname) / sizeof(keyname[0]);

int SDL_PushEvent(SDL_Event *ev) {
  printf("Not Supported: SDL_PushEvent\n");
  assert(0);
  return 0;
}

int find_key(char *key_name) {
  for (int i = 0; i < keyname_size; i++)
  {
    if (strcmp(key_name, keyname[i]) == 0) {
      return i;
    }
  }
  printf("error key: %s\n", key_name);
  return -1;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[30] = {0};
  NDL_PollEvent(buf, 30);
  if (buf[0] == 'k') {
    if (buf[1] == 'd') {
      ev->type = SDL_KEYDOWN;
      ev->key.type = SDL_KEYDOWN;
      ev->key.keysym.sym = find_key(buf + 3);
      if (ev->key.keysym.sym != -1) {
        return 1;
      }
    }
    else if (buf[1] == 'u') {
      ev->type = SDL_KEYUP;
      ev->key.type = SDL_KEYUP;
      ev->key.keysym.sym = find_key(buf + 3);
      if (ev->key.keysym.sym != -1) {
        return 1;
      }
    }
    else {
      printf("error event: %s\n", buf);
      return 0;
    }
  }
  else {
    ev->key.keysym.sym = SDLK_NONE;
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  event->key.keysym.sym = SDLK_NONE;
  while (event->key.keysym.sym == SDLK_NONE || event->key.keysym.sym == -1)
    SDL_PollEvent(event);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf("Not Supported: SDL_PeepEvents\n");
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;
  SDL_PollEvent(&ev);
  if (ev.key.type == SDL_KEYDOWN)
    keystate[ev.key.keysym.sym] = 1;
  else if (ev.key.type == SDL_KEYUP)
    keystate[ev.key.keysym.sym] = 0;
  if (numkeys) {
    *numkeys = 0;
  }
  for (int i = 0; i < keyboard_length; i++)
  {
    if (keystate[i] == 1) {
      if (numkeys) {
        *numkeys += 1;
      }
    }
  }
  return keystate;
}
