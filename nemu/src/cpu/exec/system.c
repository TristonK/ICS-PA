#include "cpu/exec.h"
void difftest_skip_ref();
void difftest_skip_dut();
uint32_t pio_read_b(ioaddr_t addr);
uint32_t pio_read_w(ioaddr_t addr);
uint32_t pio_read_l(ioaddr_t addr);
void pio_write_l(ioaddr_t addr,uint32_t data);
void pio_write_w(ioaddr_t addr,uint32_t data);
void pio_write_b(ioaddr_t addr,uint32_t data);
void raise_intr(uint8_t NO,vaddr_t ret_addr);

make_EHelper(lidt) {
//  TODO();尚未考虑IDTR.limit
//		rtl_li(&t0,id_dest->val);
//		rtl_addi(&t1,&t0,2);
//		rtl_lm(&t2,&t0,2);
//		rtl_lm(&t3,&t1,4);
//		cpu.IDTR.limit=(uint16_t)t2;
//		cpu.IDTR.base=t3;

//之前的：
//		rtl_li(&cpu.IDTR.base,id_dest->val);
//修改不知道对不对
		cpu.IDTR.base=vaddr_read(id_dest->addr+2,4);
	if(id_dest->width==2)
			rtl_andi(&cpu.IDTR.base,&cpu.IDTR.base,0x00ffffff);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
 // TODO();
	raise_intr(id_dest->val,decoding.seq_eip);
  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
 // TODO();
	rtl_pop(&decoding.jmp_eip);
	rtl_pop(&cpu.cs);
	rtl_pop(&cpu.eflags);
	rtl_j(decoding.jmp_eip);
  print_asm("iret");
}

make_EHelper(in) {
  //TODO();
	if(id_dest->width==1)t0=pio_read_b(id_src->val);
	else if(id_dest->width==2)t0=pio_read_w(id_src->val);
	else t0=pio_read_l(id_src->val);
	operand_write(id_dest,&t0);
	print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
} 

make_EHelper(out) {
//  TODO();
	if(id_src->width==1)pio_write_b(id_dest->val,id_src->val);
	else if(id_src->width==2)pio_write_w(id_dest->val,id_src->val);
	else pio_write_l(id_dest->val,id_src->val);

  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
