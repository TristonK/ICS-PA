#include "proc.h"

#define DEFAULT_ENTRY 0x8048000

#include <sys/types.h>
int fs_open(const char *pathname, int flags ,int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
int _protect(_Protect *p);
void* new_page(size_t nr_page);
//uintptr_t heapstart;

static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
// size_t len = get_ramdisk_size();
int fd=fs_open(filename,0,0);
 size_t filesize=fs_filesz(fd);
 uintptr_t va=0x8048000;
 uintptr_t end=DEFAULT_ENTRY+filesize;
 for(;va<end;va+=PGSIZE){
	void *pa=new_page(1);
//	_map(&pcb->as,(void*)va,pa,1);
    uintptr_t readsize=(end-va>PGSIZE)?PGSIZE:end-va;
	fs_read(fd,pa,readsize);
	_map(&(pcb->as),(void *)va,pa,1);  
 }
 current->cur_brk = current->max_brk= va;
// heapstart=va;
// fs_read(fd,buf,filesize);
 fs_close(fd);
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
  _protect(&pcb->as);
	uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
