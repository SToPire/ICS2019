#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  if(id_dest->type==OP_TYPE_REG) rtl_lr(&s0,id_dest->reg,id_dest->width);
  else if(id_dest->type==OP_TYPE_MEM) rtl_lm(&s0,&id_dest->addr,id_dest->width);
  else s0=id_dest->val;
  rtl_push(&s0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&s0);
  if(id_dest->type==OP_TYPE_REG) rtl_sr(id_dest->reg,&s0,id_dest->width);
  else if(id_dest->type==OP_TYPE_MEM) rtl_sm(&id_dest->addr,&s0,id_dest->width);
  else assert(0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  uint32_t  temp=cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&temp);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
  uint32_t  temp=0;
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&temp);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    if(((reg_w(R_AX)>>15)&1)==1) reg_w(R_DX)=0xFFFF;
    else reg_w(R_DX)=0;
  }
  else {
    if(((reg_l(R_EAX)>>31)&1)==1) reg_l(R_EDX)=0xFFFFFFFF;
    else reg_l(R_EDX)=0;
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    if(((reg_b(R_AL)>>7)&1)==1) reg_b(R_AH)=0xFF;
    else reg_b(R_AH)=0x00;
  }
  else {
    if(((reg_w(R_AX)>>15)&1)==1) reg_l(R_EAX)=0xFFFF0000|reg_l(R_EAX);
    else reg_l(R_EAX)=0x00000000|reg_l(R_EAX);
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
make_EHelper(movsb)
{
    rtl_lm(&s0, &cpu.esi, 1);
    rtl_sm(&cpu.edi, &s0, 1);
    ++cpu.esi;
    ++cpu.edi;

    print_asm_template2(movsb);
}
make_EHelper(movs)
{
    if (decinfo.isa.is_operand_size_16) {
        rtl_lm(&s0, &cpu.esi, 2);
        rtl_sm(&cpu.edi, &s0, 2);
        cpu.esi += 2;
        cpu.edi += 2;
    } else {
        rtl_lm(&s0, &cpu.esi, 4);
        rtl_sm(&cpu.edi, &s0, 4);
        cpu.esi += 4;
        cpu.edi += 4;
    }

    print_asm_template2(movs);
}
