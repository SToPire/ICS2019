#include <am.h>
#include <amdev.h>
#include <nemu.h>
#define W 400
#define H 320
void draw_sync();
size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = W;
      info->height = H;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
      int x1=ctl->x;
      int y1=ctl->y;
      int w1=ctl->w;
      int h1=ctl->h;
      uint32_t *pixels1=ctl->pixels;
      if(x1+w1>W) w1=W-x1;
      if(y1+h1>W) h1=H-y1;
      for(int i=0;i<h1;i++)
      {
         for(int j=0;j<w1;j++)
         {
            *(fb+(y1+i)*W+x1+j)=*(pixels1);
            pixels1+=1;
         }
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size =H * W;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
