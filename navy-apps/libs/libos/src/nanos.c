#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

#if defined(__ISA_X86__)
intptr_t _syscall_(int type, intptr_t a0, intptr_t a1, intptr_t a2){
  int ret = -1;
  asm volatile("int $0x80": "=a"(ret): "a"(type), "b"(a0), "c"(a1), "d"(a2));
  return ret;
}
#elif defined(__ISA_AM_NATIVE__)
intptr_t _syscall_(int type, intptr_t a0, intptr_t a1, intptr_t a2){
  intptr_t ret = 0;
  asm volatile("call *0x100000": "=a"(ret): "a"(type), "S"(a0), "d"(a1), "c"(a2));
  return ret;
}
#else
#error _syscall_ is not implemented
#endif

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
  while (1);
}

int _open(const char *path, int flags, mode_t mode) {
  int ret=_syscall_(SYS_open,(intptr_t)path,flags,mode);
  return ret;
}

int _write(int fd, void *buf, size_t count){
   int  ret= _syscall_(SYS_write,fd,(intptr_t)buf,count);
	//  _exit(SYS_write);
  return ret;
}
extern intptr_t _end;
void* program_end= &_end;
void *_sbrk(intptr_t increment){
   // program_end += increment;
    void * ret= program_end;
	program_end=(program_end+increment);
    _syscall_(SYS_brk,(intptr_t)program_end,0,0);
    return ret;
	// return (void *)-1;
}

int _read(int fd, void *buf, size_t count) {
 // _exit(SYS_read);
   int ret=_syscall_(SYS_read,fd,(intptr_t)buf,count);
   	return ret;
}

int _close(int fd) {
  _syscall_(SYS_close,fd,0,0);
	// _exit(SYS_close);
  return 0;
}

off_t _lseek(int fd, off_t offset, int whence) {
   int  ret= _syscall_(SYS_lseek,fd,offset,whence);
   return ret;
   	// _exit(SYS_lseek);
//  return 0;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  _syscall_(SYS_execve,(intptr_t)fname,0,0);
	//_exit(SYS_execve);
  return 0;
}
// The code below is not used by Nanos-lite.
// But to pass linking, they are defined as dummy functions

int _fstat(int fd, struct stat *buf) {
  return 0;
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}
