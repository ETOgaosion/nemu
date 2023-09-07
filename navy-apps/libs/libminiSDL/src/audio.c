#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CONFIG_SB_SIZE 0x10000


int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
  if (obtained) {
    memcpy((void *)obtained, (const void *)desired, sizeof(SDL_AudioSpec));
    obtained->size = CONFIG_SB_SIZE;
    obtained->callback = NULL;
    obtained->userdata = NULL;
  }
  return 0;
}

void SDL_CloseAudio() {
  NDL_CloseAudio();
}

void SDL_PauseAudio(int pause_on) {
  NDL_PauseAudio(pause_on);
}

#define ADJUST_VOLUME(s, v)	(s = (s * v) / SDL_MIX_MAXVOLUME)

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  int16_t src1, src2;
  int dst_sample;
  const int max_audioval = ((1 << (16 - 1)) - 1);
  const int min_audioval = -(1 << (16 - 1));

  len /= 2;
  while (len--) {
      src1 = ((src[1]) << 8 | src[0]);
      ADJUST_VOLUME(src1, volume);
      src2 = ((dst[1]) << 8 | dst[0]);
      src += 2;
      dst_sample = src1 + src2;
      if (dst_sample > max_audioval) {
          dst_sample = max_audioval;
      } else if (dst_sample < min_audioval) {
          dst_sample = min_audioval;
      }
      dst[0] = dst_sample & 0xFF;
      dst_sample >>= 8;
      dst[1] = dst_sample & 0xFF;
      dst += 2;
  }
  NDL_PlayAudio(dst, len);
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  FILE *fp = fopen(file, "r");
  WaveFMT format;
  fseek(fp, 20, SEEK_SET);
  fread(&format, sizeof(WaveFMT), 1, fp);
  spec->channels = (uint8_t)format.channels;
  spec->samples = 4096;
  spec->freq = format.frequency;
  switch(format.bitspersample) {
    case 4:
      spec->format = AUDIO_S16;
      break;
    case 8:
      spec->format = AUDIO_U8;
      break;
    case 16:
      spec->format = AUDIO_S16;
      break;
    default:
      break;
  }
  fseek(fp, 40, SEEK_SET);
  fread(audio_len, 4, 1, fp);
  *audio_buf = (uint8_t *)malloc(*audio_len);
  fread(*audio_buf, 1, *audio_len, fp);
  return spec;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  free(audio_buf);
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
