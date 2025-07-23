#include <am.h>
#include <nemu.h>
#include <stdio.h>
#define SIZE_ADDR (VGACTL_ADDR + 0)
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  // int w = inw(SIZE_ADDR + 2);  // TODO: get the correct width
  // int h = inw(SIZE_ADDR + 0);  // TODO: get the correct height
  // printf("w:%d, h:%d\n", w, h);
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(SIZE_ADDR + 2), .height = inw(SIZE_ADDR + 0),
    .vmemsz = (uint32_t)inw(SIZE_ADDR + 2) * (uint32_t)inw(SIZE_ADDR + 0) * sizeof(uint32_t)
  };
}
// 程序调用它绘制图像时，是完整绘制一帧(w*h*32bite，也是显存的大小)之后
// 才将ctl->sync置为true.然后nemu模拟的硬件将显存中的内容同步到屏幕上
// 前几次绘制sync都为false,只是将内容写到显存中
// io_write(AM_GPU_FBDRAW, x * w, y * h, color_buf, w, h, false);
// AM_DEVREG(11, GPU_FBDRAW,   WR, int x, y; void *pixels; int w, h; bool sync);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pixels = (uint32_t *)ctl->pixels;

  int y;
  int x;
  int w = ctl->w;
  int h = ctl->h;
  int screen_w = inw(SIZE_ADDR + 2);
  // printf("pixels[10]:%x\n", *(pixels+10));
  // printf("pixels[0]:%x\n", *pixels);
  for(y = ctl->y; y < ctl->y+h; y++) {
    int i = y * screen_w;
    for (x = ctl->x; x < ctl->x+w; x++) {
      fb[x+i] = *pixels;
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
