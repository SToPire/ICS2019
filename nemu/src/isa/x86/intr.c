#include "rtl/rtl.h"
#include "cpu/exec.h"

#define IRQ_TIMER 32
void raise_intr(uint32_t NO, vaddr_t ret_addr)
{
    /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
    rtl_push(&cpu.EFLAGS);
    rtl_push(&cpu.cs);
    rtl_push(&ret_addr);
    if(NO==IRQ_TIMER)
        cpu.eflags.IF = 0;
    uint32_t index = NO * 8;
    vaddr_t target_address =
        (vaddr_read(cpu.IDTR.head + index + 4, 4) & 0xFFFF0000) |
        (vaddr_read(cpu.IDTR.head + index, 4) & 0x0000FFFF);
    rtl_j(target_address);
}

bool isa_query_intr(void)
{
    Log("cas");
    if (cpu.INTR) {
        cpu.INTR = false;
        raise_intr(IRQ_TIMER, decinfo.seq_pc);
        return true;
    }
    return false;
}
