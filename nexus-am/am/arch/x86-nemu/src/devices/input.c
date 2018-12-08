#include <am.h>
#include <x86.h>
#include <amdev.h>
//port: 0x60
size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      uint32_t k=inl(0x60);
      if((k&0x8000)>0)kbd->keydown=1;
      else kbd->keydown = 0;
     // if(inl(0x60)==0)kbd->keycode=_KEY_NONE;
       kbd->keycode = k&0x7fff;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
