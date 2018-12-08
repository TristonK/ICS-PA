#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>

#define SCREEN_PORT 0x100
#define WIDTH 400
#define HEIGHT 300
static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;

size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
      uint32_t vga_info=inl(SCREEN_PORT);
      info->width = (vga_info>>16)&0xffff;
      info->height = vga_info&0xffff;
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
	  int co_x,co_y,co_h,co_w;
		co_x=ctl->x;
		co_y=ctl->y;
		co_h=ctl->h;
		co_w=ctl->w;
		uint32_t *pixel=ctl->pixels;
//		int i;
//		int size=screen_width()*screen_height();
//		for(i=0;i<size;i++)fb[i]=i;

		for(int i=0;i<co_h && (co_y+i)<HEIGHT;i++){
			for(int j=0;j<co_w && (co_x+j<WIDTH);j++){
				fb[(co_y+i)*WIDTH+co_x+j]=pixel[i*co_w+j];
			}
		}
/*		uint32_t cp_bytes;
		if(co_w<(WIDTH-co_x))cp_bytes=sizeof(uint32_t)*co_w;
		else cp_bytes=sizeof(uint32_t)*(WIDTH-co_x);
		for(int j=0;j<co_h&& j+co_y<HEIGHT;j++){
			memcpy(&fb[(co_y+j)*WIDTH+co_x],pixel,cp_bytes);
			pixel+=co_w;
		}
*/
      if (ctl->sync) {
        // do nothing, hardware syncs.
      }
      return sizeof(_FBCtlReg);
    }
  }
  return 0;
}

void vga_init() {
}
