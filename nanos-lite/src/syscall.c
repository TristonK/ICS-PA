#include "common.h"
#include "syscall.h"
#include <sys/types.h>
extern intptr_t _end;
//static intptr_t _fsh_write(intptr_t fd, char * buf, intptr_t count);
size_t fs_write(int fd,void *buf,size_t len);
ssize_t fs_read(int fd,void *buf,size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
off_t fs_lseek(int fd,off_t offset,int whence);
int fs_open(const char *pathname,int flags,int mode);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
  	case SYS_yield: /*printf("SYS_yield\n")*/;_yield();c->GPR1=0;break;
  	case SYS_exit: /*printf("SYS_exit\n")*/;_halt(a[1]);c->GPR1=0;break;
  	case SYS_write:/* printf("SYS_write\n");*/
						c->GPR1=fs_write(a[1],(void *)a[2],a[3]);
						break;
  	case SYS_brk: 	c->GPR1=0;break;
  	case SYS_open: c->GPR1=fs_open((const char *)a[1],a[1],a[2]);break;
  	case SYS_read: c->GPR1=fs_read(a[1],(void *)a[2],a[3]);break;
  	case SYS_close: c->GPR1=fs_close(a[1]);break;
  	case SYS_lseek: c->GPR1=fs_lseek(a[1],a[2],a[3]);break;

 	default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
//done by mine未考虑error
/*static intptr_t _fsh_write(intptr_t fd, char* buf, intptr_t count){
	if(fd==1||fd==2)
//		while(*buf)_putc(*buf++);
//	for(int i=0;i<10;i++)_putc(buf[i]);
//	else 
//		memcpy(fd, buf, count);
		printf(buf);
//		printf("count:%d _end:%d\n",count,_end);
	return count;
}*/
