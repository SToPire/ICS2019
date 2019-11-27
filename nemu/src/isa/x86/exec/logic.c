#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&s0,&id_src->val,&id_dest->val);
  s1=0;
  rtl_update_ZFSF(&s0,id_dest->width);
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&s0,&id_dest->val,&id_src->val);
  s1=s0;
  s0=0;
  operand_write(id_dest,&s1);
  rtl_update_ZFSF(&s1,id_dest->width);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  print_asm_template2(and);
}

make_EHelper(xor) {
    // s0 = dest xor src
  rtl_xor(&s0, &id_dest->val, &id_src->val);
  // s1 = s0
  s1=s0;
  s0=0;
  operand_write(id_dest, &s1);
  rtl_update_ZFSF(&s1, id_dest->width);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // s0 = dest or src
  rtl_or(&s0, &id_dest->val, &id_src->val);
  // s1 = s0
  s1=s0;
  s0=0;
  operand_write(id_dest, &s1);
  rtl_update_ZFSF(&s1, id_dest->width);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(rol) {
  uint32_t time1=id_src->val;
  s0=id_dest->val;
  while(time1!=0)
  {
    uint32_t kb=s0>>(id_dest->width-1)&0x1;
    s0=s0*2+kb;
    time1--;
  }
  operand_write(id_dest,&s0);
  if(id_src->val==1)
  {
    uint32_t kb=s0>>(id_dest->width-1)&0x1;
    if(kb==cpu.EFLAGS.CF)  s1=0;
    else s1=1;
    rtl_set_OF(&s1);
  }
  print_asm_template2(rol);
}

make_EHelper(ror) {
  uint32_t time1=id_src->val;
  s0=id_dest->val;
  while(time1!=0)
  {
    uint32_t kb=(s0&0x1)<<(id_dest->width-1);
    s0=s0/2+kb;
    time1--;
  }
  operand_write(id_dest,&s0);
  if(id_src->val==1)
  {
    uint32_t kb=s0>>(id_dest->width-1)&0x1;
    if(kb==cpu.EFLAGS.CF)  s1=0;
    else s1=1;
    rtl_set_OF(&s1);
  }
  print_asm_template2(ror);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  if(id_dest->width==4) s1=-1;
  else s1=(1<<(id_dest->width*8))-1;
  rtl_xor(&s0,&id_dest->val,&s1);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  print_asm_template1(not);
}
