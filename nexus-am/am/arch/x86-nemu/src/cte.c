#include <am.h>
#include <x86.h>
//my
#include <klib.h>
//my

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();
void vecsys();
void get_cur_as(_Context *c);
void _switch(_Context *c);

_Context* irq_handle(_Context *tf) {
//  printf("go here");
/*  printf("1   %d\n ",tf->edi);
  printf("2   %d\n ",tf->esi);
  printf("3   %d\n ",tf->ebp);
  printf("4   %d\n ",tf->esp);
  printf("5   %d\n ",tf->ebx);
  printf("6   %d\n ",tf->edx);
  printf("7   %d\n ",tf->ecx);
  printf("8   %d\n ",tf->eax);
  printf("err %d \n", tf->err);
  printf("irq %d\n",tf->irq);
  printf("eip %d\n",tf->eip);*/
	get_cur_as(tf);
  _Context *next = tf;
///  printf("%d\n",tf->irq);
  if (user_handler) {
    _Event ev;
    switch (tf->irq) {
	  case 0x80: ev.event= _EVENT_SYSCALL;/*printf("0x80\n");*/break;
	  case 0x81: ev.event= _EVENT_YIELD;break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }
 // printf("is go");
 // printf("%d",tf->eax);
  _switch(next);
  return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys,  DPL_KERN);
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
    uintptr_t ret=(uintptr_t)stack.end;
	//uintptr pro_size = sizeof(_Protect);
	//ret-=pro_size;memset((uintprt *)ret,0,pro_size);
	uintptr_t cont_size=sizeof(_Context);
	ret -= cont_size;
    _Context nc;//new context
    //nc.prot = 0;
   	nc.edi = 0;
	nc.esi = 0;
	nc.ebp = (uintptr_t)stack.end;
	nc.esp = nc.ebx = nc.edx = nc.ecx = nc.eax = 0;
	nc.irq = 0;
	nc.err=0;
	nc.eip=(uintptr_t)entry;
	nc.cs=0x8;
	nc.eflags=0x2;
	*(_Context *)ret=nc;
	return (_Context*)ret;
  	//return NULL;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
