#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <NDL.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

static struct timeval boot_time = {0};

time_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec - boot_time.tv_sec) * 1000 + (tv.tv_usec - boot_time.tv_usec) / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  return read(fd, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  else {
    if (*w ==0 && *h ==0)
    {
      *w = screen_w;
      *h = screen_h;
      canvas_w = screen_w;
      canvas_h = screen_h;
    }
    if (*w <= screen_w && *h <= screen_h)
    {
      canvas_w = *w;
      canvas_h = *h;
    }
    else {
      printf("canvas (w: %d, h: %d) too large, back to screen size (w: %d, h: %d)!\n", *w, *h, screen_w, screen_h);
      canvas_w = screen_w;
      canvas_h = screen_h;
    }
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int center_x = (screen_w - canvas_w) / 2 + x;
  int center_y = (screen_h - canvas_h) / 2 + y;
  int fb = open("/dev/fb", 0, 0);
  for (int i = 0; i < h; i++) {
    lseek(fb, (center_y + i) * screen_w + center_x, SEEK_SET);
    write(fb, pixels + w * i, w);
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
  int audio_ctl = open("/dev/sbctl", 0, 0);
  int buf[4] = {0, freq, channels, samples};
  write(audio_ctl, (void *)buf, 4);
}

void NDL_CloseAudio() {
}

void NDL_PauseAudio(int pause_on) {
  int audio_ctl = open("/dev/sbctl", 0, 0);
  int buf[4] = {pause_on, 0, 0, 0};
  write(audio_ctl, (void *)buf, 4);
}

int NDL_PlayAudio(void *buf, int len) {
  int audio = open("/dev/sb", 0, 0);
  return write(audio, buf, len);
}

int NDL_QueryAudio() {
  int ret = 0;
  int audio_ctl = open("/dev/sbctl", 0, 0);
  read(audio_ctl, &ret, 1);
  return ret;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  struct timeval tv;
  gettimeofday(&tv, NULL);
  boot_time.tv_sec = tv.tv_sec;
  boot_time.tv_usec = tv.tv_usec;
  char buf[35] = "";
  int fd = open("/proc/dispinfo", 0, 0);
  int res = read(fd, buf, 30);
  sscanf(buf, "WIDTH: %d\nHEIGHT: %d\n", &screen_w, &screen_h);
  return 0;
}

void NDL_Quit() {
}
