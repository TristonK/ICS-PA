#include "common.h"
#include "syscall.h"
#include <sys/types.h>

int fs_open(const char* pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, const void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset,int whence);
int fs_close(int fd);
int fs_execve(const char*filename,char*const argv[],char*const envp[]);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
//  Log("%d  %d  %d  %d\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
	case SYS_exit: /*_halt(a[1]);c->GPR1=0;*/
	//	_Context temp;
		c->GPR1=SYS_execve;
		c->GPR2=(intptr_t)"bin/init";
		c->GPR3=c->GPR4=0;
		do_syscall(c);break;
	case SYS_yield: _yield();c->GPR1=0;break;
    case SYS_open: c->GPR1=fs_open((char *)a[1],0,0);break;
    case SYS_read: c->GPR1=fs_read(a[1],(char*)a[2],a[3]);break;
 	case SYS_write:c->GPR1= fs_write(a[1],(char *)a[2],a[3]);break;
    //*****SYS_write****	
	//intptr_t ret=0;
/*	if((int)a[1]==1||(int)a[1]==2){
	  // c->GPR1=a[1]==1?a[3]:-1;
	   void *buf=(void *)a[2];
	   for(int i=0;i<a[3];i++){
	      _putc((char)*((char*)(buf+i)));
	   }
	}*/
//	Log();
   /* else{
	   c->GPR1= fs_write(a[1],(char *)a[2],a[3]);
	}
	break;*/
	//c->GPR1=ret;
	//******************
	case SYS_close: c->GPR1=fs_close(a[1]);break;
    case SYS_lseek: c->GPR1=fs_lseek(a[1],a[2],a[3]);break;
	case SYS_brk:/*Log("%d",a[1]);*/ c->GPR1=0; break;
    case SYS_execve: fs_execve((char*)a[1],0,0);break;
	default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
