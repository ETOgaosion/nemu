#include <am.h>
#include <amdev.h>
#include <common.h>
#include <device.h>
#include <klib-macros.h>
#include <klib.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {[AM_KEY_NONE] = "NONE", AM_KEYS(NAME)};

size_t null_read(void *buf, size_t offset, size_t len) {
    return 0;
}
size_t null_write(const void *buf, size_t offset, size_t len) {
    return len;
}

size_t zero_read(void *buf, size_t offset, size_t len) {
    memset(buf, 0, len);
    return len;
}
size_t zero_write(const void *buf, size_t offset, size_t len) {
    return len;
}

size_t serial_write(const void *buf, size_t offset, size_t len) {
    char *write_buf = (char *)buf;
    for (int i = 0; i < len; i++) {
        putch(write_buf[i]);
    }
    return len;
}

#define NAMEINIT(key) [AM_KEY_##key] = #key,
static const char *names[] = {AM_KEYS(NAMEINIT)};

size_t events_read(void *buf, size_t offset, size_t len) {
    AM_INPUT_KEYBRD_T ev = {0};
    ev = io_read(AM_INPUT_KEYBRD);
    if (ev.keycode == AM_KEY_NONE) {
        return 0;
    }
    size_t ret = 0;
    if (ev.keydown) {
        ret = snprintf(buf, len, "kd %s", names[ev.keycode]);
    } else {
        ret = snprintf(buf, len, "ku %s", names[ev.keycode]);
    }
    return ret;
}

static int display_width = 0;
static int display_height = 0;

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
    if (!display_height || !display_width) {
        display_width = io_read(AM_GPU_CONFIG).width;
        display_height = io_read(AM_GPU_CONFIG).height;
    }
    size_t ret = 0;
    ret += snprintf(buf, len, "WIDTH: %d\n", display_width);
    ret += snprintf(buf + ret, len, "HEIGHT: %d\n", display_height);
    return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
    int actual_start_x = offset % display_width, actual_start_y = offset / display_width;
    if (offset + len > display_width * display_height) {
        len = display_width * display_height - offset;
    }
    int actual_width = (len - 1) % display_width + 1, actual_height = (len - 1) / display_width + 1;
    void *write_buf = (void *)buf;
    io_write(AM_GPU_FBDRAW, actual_start_x, actual_start_y, write_buf, actual_width, actual_height, true);
    return len;
}

size_t sbctl_read(void *buf, size_t offset, size_t len) {
    int *sbctl_args = (int *)buf;
    sbctl_args[0] = io_read(AM_AUDIO_STATUS).count;
    return 1;
}

size_t sbctl_write(const void *buf, size_t offset, size_t len) {
    assert(len == 3);
    int *sbctl_args = (int *)buf;
    int freq = sbctl_args[0], channels = sbctl_args[1], samples = sbctl_args[2];
    io_write(AM_AUDIO_CTRL, freq, channels, samples);
    return 3;
}

size_t sb_write(const void *buf, size_t offset, size_t len) {
    Area sbuf;
    int max_buf = io_read(AM_AUDIO_CONFIG).bufsize;
    sbuf.start = (void *)buf;
    int nplay = 0;
    while (nplay < len) {
        int play_len = (len - nplay > max_buf ? max_buf : len - nplay);
        sbuf.end = sbuf.start + play_len;
        io_write(AM_AUDIO_PLAY, sbuf);
        sbuf.start += play_len;
        nplay += play_len;
    }
    return len;
}

void init_device() {
    Log("Initializing devices...");
    ioe_init();
}
