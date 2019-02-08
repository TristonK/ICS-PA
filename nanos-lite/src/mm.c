#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
	if(new_brk>=0x1004d000)printf("in mm_brk,too big");	
	uintptr_t *cur= &current->cur_brk;
	uintptr_t *max= &current->max_brk;
//	if(*max>=(0x1004e000-10*PGSIZE))printf("find in mm_brk\n");
	*cur=(new_brk&0xfff)?new_brk+PGSIZE:new_brk;	 
//	printf("brk_in%x cur%x\n",new_brk,current->cur_brk);
	while(*cur > *max){
		_map(&current->as,(void *)(*max),new_page(1),1);
		*max+=PGSIZE;
	}
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
