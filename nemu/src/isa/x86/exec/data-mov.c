#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(push_r) {
  //TODO();
  rtl_push(&reg_l(5));
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&reg_l(id_dest->reg));
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
  //TODO();
  cpu.esp=cpu.ebp;
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
    int x=reg_w(0);
    if(x<0)reg_w(2)=0xffff;
    else reg_w(2)=0x0000;
  }
  else {
    //TODO(); 
    int x=reg_l(0);
    if(x<0) reg_l(2)=0xffffffff;
    else reg_l(2)=0x0000;
    
  } 

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    int x=reg_w(0);
    if(x<0)reg_w(0)|=0xff00;
    else reg_w(0)&=0x00ff;
  }
  else {
    int x=reg_l(0);
    if(x<0) reg_l(0)|=0xffff0000;
    else reg_l(0)&=0x0000ffff;
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
