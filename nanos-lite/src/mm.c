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
//extern uintptr_t heapstart;
/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
	if(current->cur_brk == 0){
		current->cur_brk = current->max_brk = new_brk;
	}
//	else{
		if(new_brk > current->max_brk){
			uintptr_t va_begin = current->max_brk;//*PGROUNDUP(current->max_brk);*/(current->max_brk+0xfff)&(~0xfff);
			uintptr_t va_end = (new_brk&0xfff)?new_brk+PGSIZE:new_brk;
			uintptr_t va=va_begin;
			for(;va < va_end; va+=PGSIZE){
				void* pa = new_page(1);
				_map(&current->as,(void*)va,pa,1);
			}
			current->max_brk = va;//new_brk;
		}
		current->cur_brk = (new_brk&0xfff)?new_brk+PGSIZE:new_brk;
//	}
	return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
