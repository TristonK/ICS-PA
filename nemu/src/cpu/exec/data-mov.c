#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  operand_write(id_dest,&t0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
	TODO();
 /* if(decoding.is_operand_size_16){
	  t0=cpu.gpr[4]._16;
	  rtl_push(&cpu.gpr[0]._16);
	  rtl_push(&cpu.gpr[1]._16);
	  rtl_push(&cpu.gpr[2]._16);
	  rtl_push(&cpu.gpr[3]._16);
      rtl_push(&t0);
	  rtl_push(&cpu.gpr[5]._16);
	  rtl_push(&cpu.gpr[6]._16);    
	  rtl_push(&cpu.gpr[7]._16);
  }
  else {
      t0=cpu.esp;
	  rtl_push(&cpu.eax);
      rtl_push(&cpu.ecx);
	  rtl_push(&cpu.edx);
	  rtl_push(&cpu.ebx);
      rtl_push(&t0);
	  rtl_push(&cpu.ebp);
	  rtl_push(&cpu.esi);
      rtl_push(&cpu.edi);
*/  
  print_asm("pusha");
}

make_EHelper(popa) { 
	TODO();
 /* if(decoding.is_operand_size_16){
	  rtl_pop(&cpu.gpr[7]._16);
	  rtl_pop(&cpu.gpr[6]._16);
	  rtl_pop(&cpu.gpr[5]._16);
	  rtl_pop(&t0);
	  rtl_pop(&cpu.gpr[3]._16);
	  rtl_pop(&cpu.gpr[2]._16);
	  rtl_pop(&cpu.gpr[1]._16);
	  rtl_pop(&cpu.gpr[0]._16);
  }
  else{
	  rtl_pop(&cpu.edi);
	  rtl_pop(&cpu.esi);
	  rtl_pop(&cpu.ebp);
	  rtl_pop(&t0);
	  rtl_pop(&cpu.ebx);
	  rtl_pop(&cpu.edx);
	  rtl_pop(&cpu.ecx);
	  rtl_pop(&cpu.eax);
  }*/
  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
	  if(cpu.gpr[0]._16<0)
		  cpu.gpr[2]._16|=0xffff;
	  else
		  cpu.gpr[2]._16=0;
  }
  else {
    if(cpu.eax<0)
		rtl_li(&cpu.edx,0xffffffff);
	else
		rtl_li(&cpu.edx,0x0000000);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
  //  rtl_lr(&t0,cpu.gpr[0]._8[0],1);
      rtl_lr(&t0,R_AL,1);
  	  rtl_sext(&t1,&t0,1);
	  rtl_sr(R_AX,&t1,2);
//	rtl_sr(cpu.gpr[0]._16,&t1,2);
  }
  else {
   // rtl_lr(&t0,cpu.gpr[0]._16,2);
      rtl_lr(&t0,R_AX,2);
  	  rtl_sext(&t1,&t0,2);
	rtl_sr(cpu.eax,&t1,4);
    // rtl_sr(R_EAX,&t1,4);
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
