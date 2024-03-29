#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
 // rtl_li(&t0,ret_addr);
 // rtl_push(&t0);
  rtl_push(&ret_addr);
  rtl_li(&t0,cpu.idtr.base);
  rtl_addi(&t0,&t0,NO*8);
  rtl_lm(&t1,&t0,4);
  rtl_addi(&t0,&t0,4);
  rtl_lm(&t2,&t0,4);
  t0=((t1&0x0000ffff)|(t2&0xffff0000));
 // rtl_j(t0); 
  decoding.jmp_eip=t0;
 // decoding.is_jmp=1; 
  rtl_j(decoding.jmp_eip);
 // decoding_set_jmp(true);
 cpu.IF=false;
}

void dev_raise_intr() {
	cpu.INTR=true;
}
