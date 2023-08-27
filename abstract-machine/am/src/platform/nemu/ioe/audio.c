#include <am.h>
#include <nemu.h>
#include <klib.h>

#define AUDIO_FREQ_ADDR (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR (AUDIO_ADDR + 0x14)

static int buf_head = 0;
static int buf_sizemax;

void __am_audio_init() {
    buf_head = 0;
    buf_sizemax = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
    cfg->present = true;
    cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
    outl(AUDIO_FREQ_ADDR, ctrl->freq);
    outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
    outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
    outl(AUDIO_INIT_ADDR, 1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
    stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
    int len = ctl->buf.end - ctl->buf.start;
    int buf_count = inl(AUDIO_COUNT_ADDR);
    int free = buf_sizemax - buf_count;
    int nwrite = len;
    while (free < len)
        ;
    if (nwrite + buf_head < buf_sizemax) {
        memcpy((void *)(uintptr_t)(AUDIO_SBUF_ADDR + buf_head), ctl->buf.start, nwrite);
        buf_head += nwrite;
    } else {
        int first_cpy_len = buf_sizemax - buf_head;
        memcpy((void *)(uintptr_t)(AUDIO_SBUF_ADDR + buf_head), ctl->buf.start, first_cpy_len);
        memcpy((void *)(uintptr_t)AUDIO_SBUF_ADDR, ctl->buf.start + first_cpy_len, nwrite - first_cpy_len);
        buf_head = nwrite - first_cpy_len;
    }
}
