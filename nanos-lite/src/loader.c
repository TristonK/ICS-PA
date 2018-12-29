#include "proc.h"

#define DEFAULT_ENTRY 0x4000000

#include <sys/types.h>
int fs_open(const char *pathname, int flags ,int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
int _protect(_Protect *p);
static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
// size_t len = get_ramdisk_size();
// Log("here");
 int fd=fs_open(filename,0,0);
// Log("fd is %d",fd);
 size_t filesize=fs_filesz(fd);
 void *buf=(void *)0x4000000;
// Log("filezie is %d",filesize);
 fs_read(fd,buf,filesize);
 fs_close(fd);
// Log("here");
 // ramdisk_read(buf,0,len);
   	return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
//	Log("here");
//  _protect(pcb);
	uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
