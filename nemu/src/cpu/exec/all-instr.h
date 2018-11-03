#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(add);
make_EHelper(or);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(and);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(cmp);

make_EHelper(push);
make_EHelper(pop);
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jmp);
make_EHelper(lea);
/*make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper();*/
make_EHelper(leave);
make_EHelper(test);
make_EHelper(movzx);
make_EHelper(setcc);
make_EHelper(nop);
make_EHelper(jcc);
