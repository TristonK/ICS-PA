#include "cpu/exec.h"
#include "device/port-io.h"
void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
//	printf("%x  *\n",id_dest->val);
 // rtl_li(&cpu.idtr,id_dest->val);
if(id_dest->width==2)
	cpu.idtr.base=vaddr_read(id_dest->addr+2,4)&&0x00ffffff;
else if(id_dest->width==4)
	cpu.idtr.base=vaddr_read(id_dest->addr+2,4);




  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
//  TODO();
// Log("r2cr %d",id_dest->reg);
   	if(id_dest->reg==0)
	  cpu.cr0.val=id_src->val;
  else if(id_dest->reg==3)
	  cpu.cr3.val=id_src->val;
  else
	  assert(0);
//	id_dest->val=id_src->val;
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
 // Log("cr2r %d",id_src->reg);
  if(id_src->reg==0){
	  t1=cpu.cr0.val;
  }
  else if(id_src->reg==3){
	  t1=cpu.cr3.val;
  }
  else 
	  assert(0);
  operand_write(id_dest,&t1);


  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  raise_intr(id_dest->val,decoding.seq_eip);//cpu.eip+2);

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
 // decoding.is_jmp=1;
  rtl_j(decoding.jmp_eip);
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
