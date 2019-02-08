#include "proc.h"
//#include "loader.c"
#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;
PCB *fg_pcb;
void naive_uload(PCB *pcb, void *entry);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb,const char* filename);
void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
   fg_pcb=&pcb[1];
   context_uload(&pcb[0],"/bin/hello");
   context_uload(&pcb[1],"/bin/pal");
   context_uload(&pcb[2],"/bin/pal");
   context_uload(&pcb[3],"/bin/pal");
   // context_kload(&pcb[0],(void *)hello_fun);
   switch_boot_pcb();
}
extern int pcb_code;
_Context* schedule(_Context *prev) {
  current->cp = prev;
//  current=&pcb[0];
  fg_pcb=&fg_pcb[pcb_code];
  current=(current==&pcb[0]?fg_pcb:&pcb[0]);
  return current->cp;
//	return NULL;
}
