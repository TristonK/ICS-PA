#include "cpu/exec.h"
#include "device/port-io.h"
void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
//	printf("%x  *\n",id_dest->val);
  rtl_li(&cpu.idtr,id_dest->val);

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
  raise_intr(id_dest->val,cpu.eip+2);

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
 // TODO();
// decoding.is_jmp=1;
   	rtl_pop(&decoding.jmp_eip);
   // decoding.is_jmp=1;
 // decoding.jmp_eip=t0;
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags);
  decoding.is_jmp=1;
 // rtl_j(decoding.jmp_eip);
  print_asm("iret");
}

make_EHelper(in) {
  //TODO();
  switch (id_dest->width){
      case 4: t2 = pio_read_l(id_src->val);break;
	  case 2: t2 = pio_read_w(id_src->val);break;
	  case 1: t2 = pio_read_b(id_src->val);break;
      default: assert(0);
  }
  operand_write(id_dest,&t2);
  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  //TODO();
  switch (id_src->width){
      case 4: pio_write_l(id_dest->val,id_src->val);break;
	  case 2: pio_write_w(id_dest->val,id_src->val);break;
	  case 1: pio_write_b(id_dest->val,id_src->val);break;
      default: assert(0);
  }
  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
