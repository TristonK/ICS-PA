#include "cpu/exec.h"
#include "memory/mmu.h"
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	rtl_push(&cpu.eflags);
	rtl_push(&cpu.cs);
	rtl_push(&ret_addr);
	uint32_t base=cpu.IDTR.base+(uint32_t)(NO)*8;
	uint32_t h=vaddr_read(base+4,4);
	uint32_t l=vaddr_read(base,4);
	uint32_t addr=(h&0xffff0000)|(l&0x0000ffff);
	rtl_li(&decoding.jmp_eip,addr);
	rtl_j(addr);
//  TODO();
}

void dev_raise_intr() {
}
