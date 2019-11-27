#include "cpu/exec.h"
void raise_intr(uint8_t NO, vaddr_t ret_addr);
make_EHelper(lidt) {
  cpu.idtr.len=vaddr_read(id_dest->addr,2);
  cpu.idtr.addr=vaddr_read(id_dest->addr+2,4);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  switch(decinfo.opcode){
  case 0xcc : raise_intr(0x3,decinfo.seq_pc);break;
  case 0xcd : raise_intr(id_dest->val,decinfo.seq_pc);break;
  case 0xce : raise_intr(0x4,decinfo.seq_pc);break;
  }
  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&s0);
  rtl_pop(&cpu.cs);    
  rtl_pop(&cpu.eflags);
  rtl_j(s0);
  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  switch(id_dest->width){
  case 1:s0=pio_read_b(id_src->val);break;
  case 2:s0=pio_read_w(id_src->val);break;
  case 4:s0=pio_read_l(id_src->val);break;
  default:assert(0);break;
  }
  rtl_sr(id_dest->reg,&s0,id_dest->width);
  print_asm_template2(in);
}

make_EHelper(out) {
  s0=id_dest->val;
  switch(id_dest->width){
  case 1:pio_write_b(s0,id_src->val);break;
  case 2:pio_write_w(s0,id_src->val);break;
  case 4:pio_write_l(s0,id_src->val);break;
  default:assert(0);break;
  }
  print_asm_template2(out);
}
