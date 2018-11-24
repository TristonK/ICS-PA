#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
 // TODO();
	
	t0=0;
	rtl_set_OF(&t0);
	rtl_set_CF(&t0);
	rtl_and(&t1,&id_dest->val,&id_src->val);
	if(t1==0)t0=1;
	else t0=0;
	rtl_set_ZF(&t0);

	rtl_msb(&t2,&t1,id_dest->width);
	rtl_set_SF(&t2);

	rtl_update_ZFSF(&t1,id_dest->width);
		
  print_asm_template2(test);
}

make_EHelper(and) {
 // TODO();
	if(id_src->type==OP_TYPE_IMM && id_src->width==1&& id_dest->width>1)
	{
		rtl_sext(&t0,&id_src->val,id_src->width);
		if(id_dest->width==2)t0=t0&0xffff;
	}
	else t0=id_src->val;
	rtl_and(&t1,&id_dest->val,&t0);
	rtl_update_ZFSF(&t1,id_dest->width);
	operand_write(id_dest,&t1);
	t1=0;
	rtl_set_CF(&t1);
	rtl_set_OF(&t1);

  print_asm_template2(and);
}

make_EHelper(xor) {
 // TODO();
	rtl_xor(&t0,&id_dest->val,&id_src->val);		
	t1=0;
	t2=0;
	rtl_set_CF(&t1);
	rtl_set_OF(&t2);	
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
	rtl_or(&t0,&id_dest->val,&id_src->val);
	t1=0;
	rtl_set_CF(&t1);
	rtl_set_OF(&t1);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  print_asm_template2(or);
}

make_EHelper(sar) {
 // TODO();
  // unnecessary to update CF and OF in NEMU
	rtl_sext(&t0,&id_dest->val,id_dest->width);
	rtl_sar(&t1,&t0,&id_src->val);
	rtl_update_ZFSF(&t1,id_dest->width);
	operand_write(id_dest,&t1);
  print_asm_template2(sar);
}

make_EHelper(shl) {
//  TODO();
  // unnecessary to update CF and OF in NEMU
	rtl_shl(&t0,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
	rtl_shr(&t0,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&t0,id_dest->width);
	operand_write(id_dest,&t0);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  //TODO();
	rtl_not(&t0,&id_dest->val);
	operand_write(id_dest,&t0);
  print_asm_template1(not);
}

make_EHelper(rol){
	//added by myself

	rtl_li(&t0,id_src->val);
	rtl_andi(&t0,&t0,0xff);
	rtl_li(&t2,id_dest->val);
	while(t0){
		rtl_msb(&t1,&t2,id_dest->width);
		rtl_mul_loi(&t2,&t2,2);
		rtl_addi(&t2,&t2,t1);	
		rtl_subi(&t0,&t0,1);
	}
		operand_write(id_dest,&t2);
		//之后只用到t2这个最终结果,t0,t1,t3可以用
	if(id_src->val==1){
		rtl_get_CF(&t0);
		rtl_msb(&t1,&t2,id_dest->width);
		if(t1!=t0)t3=1;
		else t3=0;
		rtl_set_OF(&t3);
	}
}
