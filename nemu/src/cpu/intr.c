#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_li(&t0,ret_addr);
  rtl_push(&t0);
  rtl_li(&t0,cpu.idtr);
  rtl_addi(&t0,&t0,NO*8);
  rtl_lm(&t1,&t0,4);
  rtl_addi(&t0,&t0,4);
  rtl_lm(&t2,&t0,4);
  t0=(t1&0x0000ffff)|(t2&0xffff0000);
  rtl_j(t0);  
}

void dev_raise_intr() {
}
