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
  //TODO();
  if (decinfo.isa.is_operand_size_16) {
    rtlreg_t x1=0x0000ffff&cpu.esp,x2=0x0000ffff&cpu.eax;
    rtl_push(&x2);
    x2=0x0000ffff&cpu.ecx;
    rtl_push(&x2);
    x2=0x0000ffff&cpu.edx;
    rtl_push(&x2);
    x2=0x0000ffff&cpu.ebx;
    rtl_push(&x2);
    rtl_push(&x1);
    x2=0x0000ffff&cpu.ebp;
    rtl_push(&x2);
    x2=0x0000ffff&cpu.esi;
    rtl_push(&x2);
    x2=0x0000ffff&cpu.edi;
    rtl_push(&x2);
  }
  else {
    rtlreg_t x1=cpu.esp;
    rtl_push(&cpu.eax);
    rtl_push(&cpu.ecx);
    rtl_push(&cpu.edx);
    rtl_push(&cpu.ebx); 
    rtl_push(&x1);
    rtl_push(&cpu.ebp);
    rtl_push(&cpu.esi);
    rtl_push(&cpu.edi);
  }
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  if (decinfo.isa.is_operand_size_16) {
    rtlreg_t x2=0,x1=0;
    rtl_push(&x2);
    cpu.gpr[7]._16=x2;
    rtl_push(&x2);
    cpu.gpr[6]._16=x2;
    rtl_push(&x2);
    cpu.gpr[5]._16=x2;
    rtl_push(&x1);
    rtl_push(&x2);
    cpu.gpr[3]._16=x2;
    rtl_push(&x2);
    cpu.gpr[2]._16=x2;
    rtl_push(&x2);
    cpu.gpr[1]._16=x2;
    rtl_push(&x2);
    cpu.gpr[0]._16=x2;
    cpu.gpr[4]._16=x1;
  }
  else {
    rtlreg_t x1=0;
    rtl_pop(&cpu.edi);
    rtl_pop(&cpu.esi);
    rtl_pop(&cpu.ebp);
    rtl_pop(&x1);
    rtl_pop(&cpu.ebx);
    rtl_pop(&cpu.edx);
    rtl_pop(&cpu.ecx);
    rtl_pop(&cpu.eax);
    cpu.gpr[4]._32=x1;
    
  }
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
make_EHelper(movsb) {
  rtl_lm(&s0,&cpu.esi,1);
  rtl_sm(&cpu.edi,&s0,1);
  cpu.esi++;cpu.edi++;
  print_asm_template2(movsb);
}
make_EHelper(movsw) {
  rtlreg_t width=decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_lm(&s0,&cpu.esi,width);
  rtl_sm(&cpu.edi,&s0,width);
  cpu.esi+=width;cpu.edi+=width;
  print_asm_template2(movsb);
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
make_EHelper(rol){
  rtlreg_t x1=8*id_dest->width-id_src->val,x2,x3,x4;
  rtl_shl(&x2,&id_dest->val,&id_src->val);rtl_shr(&x3,&id_dest->val,&x1);
  rtl_or(&x4,&x2,&x3);
  operand_write(id_dest,&x4);/*switch(id_dest->width){
    case 1:x2=(x1<<id_src->val)&(uint32_t)0xff;x2|=(x1>>(8-id_src->val));break;
    case 2:x2=(x1<<id_src->val)&(uint32_t)0xffff;x2|=(x1>>(16-id_src->val));break;
    case 4:x2=(x1<<id_src->val);x2|=(x1>>(32-id_src->val));break;
  }*/
  print_asm_template2(rol);
}
