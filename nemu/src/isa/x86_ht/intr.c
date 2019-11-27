#include "rtl/rtl.h"
void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.Eflags);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  vaddr_t target=(vaddr_read(cpu.idtr.base+NO*8+6,2)<<16)|(vaddr_read(cpu.idtr.base+NO*8,2));
  interpret_rtl_j(target);
}

bool isa_query_intr(void) {
  return false;
}
