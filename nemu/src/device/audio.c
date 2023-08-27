/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more desbuf_tails.
 ***************************************************************************************/

#include "debug.h"
#include <SDL2/SDL.h>
#include <common.h>
#include <device/map.h>

enum {
    reg_freq,
    reg_channels,
    reg_samples,
    reg_sbuf_size,
    reg_init,
    reg_count,
    nr_reg
};

static uint8_t *sbuf = NULL;
static int sbuf_tail = 0;
static int sbuf_count = 0;
static uint32_t *audio_base = NULL;

SDL_AudioSpec s;
SDL_AudioDeviceID dev;

static void buf_maintain(uint32_t offset, int len, bool is_write) {
    Assert(is_write, "[audio] read audio mem");
    sbuf_count += len;
}

static void audio_play(void *userdata, uint8_t *stream, int len) {
    int nread = len;
    if (CONFIG_SB_SIZE - sbuf_count < len) {
        nread = CONFIG_SB_SIZE - sbuf_count;
    }

    if (nread + sbuf_tail < CONFIG_SB_SIZE) {
        memcpy(stream, sbuf + sbuf_tail, nread);
        sbuf_tail += nread;
    } else {
        int first_cpy_len = CONFIG_SB_SIZE - sbuf_tail;
        memcpy(stream, sbuf + sbuf_tail, first_cpy_len);
        memcpy(stream + first_cpy_len, sbuf, nread - first_cpy_len);
        sbuf_tail = nread - first_cpy_len;
    }
    sbuf_count -= nread;
    if (len > nread) {
        memset(stream + nread, 0, len - nread);
    }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
    bool handled = false;
    switch (offset) {
    case reg_freq * 4:
        if (len == 4) {
            if (is_write) {
                s.freq = audio_base[reg_freq];
            } else {
                audio_base[reg_freq] = s.freq;
            }
            handled = true;
        }
        break;
    case reg_channels * 4:
        if (len == 4) {
            if (is_write) {
                s.channels = audio_base[reg_channels];
            } else {
                audio_base[reg_channels] = s.channels;
            }
            handled = true;
        }
        break;
    case reg_samples * 4:
        if (len == 4) {
            if (is_write) {
                s.samples = audio_base[reg_samples];
            } else {
                audio_base[reg_samples] = s.samples;
            }
            handled = true;
        }
        break;
    case reg_sbuf_size * 4:
        if (len == 4) {
            if (!is_write) {
                audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
            }
            handled = true;
        }
        break;
    case reg_init * 4:
        if (len == 4 && is_write && audio_base[reg_init]) {
            s.samples = audio_base[reg_samples];
            s.freq = audio_base[reg_freq];
            s.channels = audio_base[reg_channels];
            s.callback = audio_play;
            handled = true;
            SDL_InitSubSystem(SDL_INIT_AUDIO);
            SDL_OpenAudio(&s, NULL);
            SDL_PauseAudio(0);
            handled = true;
        }
        break;
    case reg_count * 4:
        if (len == 4) {
            if (!is_write) {
                audio_base[reg_count] = sbuf_tail;
                handled = true;
            }
        }
        break;
    default:
        break;
    }
    if (!handled) {
        panic("[Audio] assess error: %u, %d, %d", offset, len, is_write);
    }
}

void init_audio() {
    uint32_t space_size = sizeof(uint32_t) * nr_reg;
    audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
    add_pio_map("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
    add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

    sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
    add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, buf_maintain);

    SDL_zero(s);
    s.format = AUDIO_S16SYS; // 假设系统中音频数据的格式总是使用16位有符号数来表示
    s.userdata = NULL;       // 不使用
    s.size = CONFIG_SB_SIZE;
}
