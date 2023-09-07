#ifndef __SDL_AUDIO_H__
#define __SDL_AUDIO_H__

/* WAVE files are little-endian */

/*******************************************/
/* Define values for Microsoft WAVE format */
/*******************************************/
#define RIFF		0x46464952		/* "RIFF" */
#define WAVE		0x45564157		/* "WAVE" */
#define FACT		0x74636166		/* "fact" */
#define LIST		0x5453494c		/* "LIST" */
#define FMT		0x20746D66		/* "fmt " */
#define DATA		0x61746164		/* "data" */
#define PCM_CODE	0x0001
#define MS_ADPCM_CODE	0x0002
#define IMA_ADPCM_CODE	0x0011
#define MP3_CODE	0x0055
#define WAVE_MONO	1
#define WAVE_STEREO	2

/* Normally, these three chunks come consecutively in a WAVE file */
typedef struct WaveFMT {
/* Not saved in the chunk we read:
	Uint32	FMTchunk;
	Uint32	fmtlen;
*/
	uint16_t	encoding;	
	uint16_t	channels;		/* 1 = mono, 2 = stereo */
	uint32_t	frequency;		/* One of 11025, 22050, or 44100 Hz */
	uint32_t	byterate;		/* Average bytes per second */
	uint16_t	blockalign;		/* Bytes per sample block */
	uint16_t	bitspersample;		/* One of 8, 12, 16, or 4 for ADPCM */
} WaveFMT;

/* The general chunk found in the WAVE file */
typedef struct Chunk {
	uint32_t magic;
	uint32_t length;
	uint8_t *data;
} Chunk;

typedef struct {
  int freq;
  uint16_t format;
  uint8_t channels;
  uint16_t samples;
  uint32_t size;
  void (*callback)(void *userdata, uint8_t *stream, int len);
  void *userdata;
} SDL_AudioSpec;

#define AUDIO_U8 8
#define AUDIO_S16 16
#define AUDIO_S16SYS AUDIO_S16

#define SDL_MIX_MAXVOLUME  128

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
void SDL_CloseAudio();
void SDL_PauseAudio(int pause_on);
SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len);
void SDL_FreeWAV(uint8_t *audio_buf);
void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume);
void SDL_LockAudio();
void SDL_UnlockAudio();

#endif
