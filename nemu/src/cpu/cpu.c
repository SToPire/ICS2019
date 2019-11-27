#include "cpu/exec.h"

CPU_state cpu;

rtlreg_t s0, s1, s2, t0, t1, ir, ZERO;

/* shared by all helper functions */
DecodeInfo decinfo;

void decinfo_set_jmp(bool is_jmp)
{
    decinfo.is_jmp = is_jmp;
}
void decinfo_set_jmp_addr(vaddr_t addr)
{
    decinfo.jmp_pc = addr;
}
void isa_exec(vaddr_t* pc);

vaddr_t exec_once(void)
{
    decinfo.seq_pc = cpu.pc;
    isa_exec(&decinfo.seq_pc);
    update_pc();

    return decinfo.seq_pc;
}
