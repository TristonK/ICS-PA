#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
 // TODO();	
	//未实现esp-2的操作。
	rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
//  TODO();

	rtl_pop(&id_dest->val);
	operand_write(id_dest,&id_dest->val);	
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
 // TODO();
	cpu.esp=cpu.ebp;
	rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    	rtl_msb(&t0,&cpu.eax,2);
	if(t0==1)cpu.edx |=0x0000ffff;
	else cpu.edx &=0xffff0000;	
  }
  else {
    //TODO();
    rtl_msb(&t0,&cpu.eax,4);
    if(t0==1)cpu.edx |=0xffffffff;
    else cpu.edx =0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
		rtl_lr(&t0,R_AL,1);
		  rtl_sext(&t1,&t0,1);
		  rtl_sr(R_AX,&t1,2);
  }
  else {
   // TODO();
   		rtl_lr(&t0,R_AX,2);
		  rtl_sext(&t1,&t0,2);
		  rtl_sr(R_EAX,&t1,4);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
