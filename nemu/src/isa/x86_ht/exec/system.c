#include "cpu/exec.h"
void raise_intr(uint32_t NO, vaddr_t ret_addr);
make_EHelper(lidt) {
  //TODO();
  if (decinfo.isa.is_operand_size_16) {
    //cpu.idtr.limit=id_src->val;
    cpu.idtr.base=0x00ffffff&id_dest->val;
  }
  else {
    //cpu.idtr.limit=id_src->val;
    cpu.idtr.base=id_dest->val;
  }
  //printf("??%d\n",cpu.idtr.base);
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
  //printf("**yes\n");
  //printf("%d\n",id_dest->val);
  raise_intr(id_dest->val,decinfo.seq_pc);
  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  //TODO();
  rtlreg_t ret_addr;
  rtl_pop(&ret_addr);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.Eflags);
  rtl_j(ret_addr);
  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  //TODO();
  //printf("%d\n",id_dest->val);
  switch(id_src->width){
    case 4:t0=pio_read_l(id_src->val);break;
    case 2:t0=pio_read_w(id_src->val);break;
    case 1:t0=pio_read_b(id_src->val);break;
    default:assert(0);
  }
  operand_write(id_dest, &t0);
  //printf("%d\n",id_dest->val);
  print_asm_template2(in);
}

make_EHelper(out) {
  //TODO();
  switch(id_dest->width){
    case 4:pio_write_l(id_dest->val,id_src->val);break;
    case 2:pio_write_w(id_dest->val,id_src->val);break;
    case 1:pio_write_b(id_dest->val,id_src->val);break;
    default:assert(0);
  }
  print_asm_template2(out);
}
