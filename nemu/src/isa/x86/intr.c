#include "rtl/rtl.h"
#include<stdio.h>
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  vaddr_t a=vaddr_read(cpu.idtr.addr+8*NO,4);
  vaddr_t b=vaddr_read(cpu.idtr.addr+8*NO+4,4);
  vaddr_t entry_addr=(a&0x0000ffff)|(b&0xffff0000);
  rtl_j(entry_addr);
}

bool isa_query_intr(void) {
  return false;
}
