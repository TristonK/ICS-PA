#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
 // Log("%d  %d  %d  %d\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
	case SYS_exit: _halt(a[1]);c->GPR1=0;break;
	case SYS_yield: _yield();c->GPR1=0;break;
    case SYS_write:
    //*****SYS_write****	
	//intptr_t ret=0;
	if((int)a[1]==1||(int)a[1]==2){
	   c->GPR1=a[1]==1?a[3]:-1;
	   void *buf=(void *)a[2];
	   for(int i=0;i<a[3];i++){
	      _putc((char)*((char*)(buf+i)));
	   }
	}
	Log();
	//c->GPR1=ret;
    break;	
	//******************
	case SYS_brk:/*Log("%d",a[1]);*/ c->GPR1=0; break;
  	default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
