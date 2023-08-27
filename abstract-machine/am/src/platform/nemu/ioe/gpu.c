#include <am.h>
#include <klib.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

int W, H;
uint32_t *fb;

void __am_gpu_init() {
    uint32_t config = inl(VGACTL_ADDR);
    W = (config >> 16) & 0xffff;
    H = config & 0xffff;
    fb = (uint32_t *)(uintptr_t)FB_ADDR;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
    *cfg = (AM_GPU_CONFIG_T){.present = true, .has_accel = false, .width = W, .height = H, .vmemsz = W * H};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
    int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
    uint32_t *pixels = ctl->pixels;
    for (int j = 0; j < h && y + j < H; j++) {
        for (int k = 0; k < w && x + k < W; k++) {
            fb[(y + j) * W + (x + k)] = *pixels;
            pixels++;
        }
    }
    if (ctl->sync) {
        outl(SYNC_ADDR, 1);
    }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
    status->ready = true;
}
