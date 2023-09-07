#ifndef __NDL_H__
#define __NDL_H__

#include <stdint.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

int NDL_Init(uint32_t flags);
void NDL_Quit();
time_t NDL_GetTicks();
void NDL_OpenCanvas(int *w, int *h);
int NDL_PollEvent(char *buf, int len);
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h);
void NDL_OpenAudio(int freq, int channels, int samples);
void NDL_CloseAudio();
void NDL_PauseAudio(int pause_on);
int NDL_PlayAudio(void *buf, int len);
int NDL_QueryAudio();

#ifdef __cplusplus
}
#endif

#endif
