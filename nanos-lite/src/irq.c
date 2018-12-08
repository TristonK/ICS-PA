#include "common.h"
//#include "syscall.h"
_Context* do_syscall(_Context *c);

 static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
   case _EVENT_YIELD:/* printf("_event_yield found\n");*/break;
	case _EVENT_SYSCALL: /*printf("_event_syscall found\n");*/
							do_syscall(c);break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
