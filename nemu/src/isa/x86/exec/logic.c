#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&s0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&s0, id_dest->width);
  rtl_set_OF(&ZERO);rtl_set_CF(&ZERO);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_sext(&s2,&id_src->val,id_src->width);
  rtl_and(&id_dest->val,&id_dest->val,&s2);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  rtl_set_OF(&ZERO);rtl_set_CF(&ZERO);
  operand_write(id_dest,&id_dest->val);

  print_asm_template2(and);
}

make_EHelper(xor) {
	rtl_sext(&s2,&id_src->val,id_src->width);
  rtl_xor(&id_dest->val,&id_dest->val,&s2);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  rtl_set_OF(&ZERO);rtl_set_CF(&ZERO);
  operand_write(id_dest,&id_dest->val);
  
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_sext(&s2,&id_src->val,id_src->width);
  rtl_or(&id_dest->val,&id_dest->val,&s2);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  rtl_set_OF(&ZERO);rtl_set_CF(&ZERO);
  operand_write(id_dest,&id_dest->val);

  print_asm_template2(or);
}

make_EHelper(sar) {
  id_dest->val = (rtlreg_t)((int32_t)id_dest->val >> id_src->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  operand_write(id_dest,&id_dest->val);
  // unnecessary to update CF and OF in NEMU
  
  print_asm_template2(sar);
}

make_EHelper(shl) {
	id_dest->val <<= id_src->val;
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  operand_write(id_dest,&id_dest->val);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
