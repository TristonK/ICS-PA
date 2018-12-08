#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>

static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;

size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
	  int ret=inl(0x100);
      info->width = ret>>16;
      info->height = ret&0xffff;
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
      /* int i;
	  int size=screen_width()*screen_height();
	  for(i=0;i<size;i++) fb[i]=i;*/
	 // int heigh=screen_height();
	  int weigh=screen_width();
	  int x=ctl->x;
	  int y=ctl->y;
	  int w=ctl->w;
	  int h=ctl->h;
	  for(int i=0;i<h;i++){
		  for(int j=0;j<w;j++){
              fb[x+j+(y+i)*weigh]=ctl->pixels[i*w+j];//x+j+(y+i)*s_wi=*(pix+cnt)
		  }
	   }
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
