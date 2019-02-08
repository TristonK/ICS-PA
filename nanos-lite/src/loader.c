#include "proc.h"
#include "memory.h"
#include "klib.h"
#include<sys/types.h>
#define DEFAULT_ENTRY 0x8048000
size_t ramdisk_read(void *buf, size_t offset, size_t len); 
size_t get_ramdisk_size();
size_t fs_write(int fd,void *buf,size_t len);
ssize_t fs_read(int fd,void *buf,size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
off_t fs_lseek(int fd,off_t offset,int whence);
int fs_open(const char *pathname,int flags,int mode);

static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
	int fd=fs_open(filename,0,0);
	int pg_num=fs_filesz(fd)/PGSIZE;
//	if(fs_filesz(fd)& ~(PGSIZE-1)){
//		pg_num=pg
//	}	
	uintptr_t paddr;
//	uintptr_t ret=0;
	uintptr_t vaddr=DEFAULT_ENTRY;
	for(int i=0;i<=pg_num;i++){
		paddr=(uintptr_t)new_page(1);
//		if(i==0)ret=paddr;
		if(i<pg_num)
			fs_read(fd,(void *)paddr,PGSIZE);
		else fs_read(fd,(void *)paddr,fs_filesz(fd)%4096);
		_map(&(pcb->as),(void *)vaddr,(void *)paddr,1);
		vaddr+=PGSIZE;
	}
	//4KB aligned
//	paddr=(uintptr_t)new_page(1);
//	vaddr=DEFAULT_ENTRY+(pg_num+1)*PGSIZE;
//	_map(&(pcb->as),(void *)vaddr,(void *)paddr,1);
	pcb->max_brk=vaddr;
	pcb->cur_brk=vaddr;
//	pcb->max_brk=vaddr+PGSIZE;
//	pcb->cur_brk=vaddr+PGSIZE;
//	printf("%x %x\n",pcb->cur_brk,pcb->max_brk);
	fs_close(fd);
	return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}
int execve(const char *filename,char *const argv[],char *const envp[]){
	naive_uload(0,filename);
	return -1;
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

