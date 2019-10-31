#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr)
{
    /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
    uint32_t index = NO * 8;
    vaddr_t target_address =
        (vaddr_read(cpu.IDTR.head + index + 4, 4) & 0xFFFF0000) |
        (vaddr_read(cpu.IDTR.head + index, 4) & 0x0000FFFF);
    decinfo_set_jmp(true);
    decinfo_set_jmp_addr(target_address);
}

bool isa_query_intr(void)
{
    return false;
}
