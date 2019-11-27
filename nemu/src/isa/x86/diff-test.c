#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state* ref_r, vaddr_t pc)
{
    uint32_t r[77];
    ref_difftest_getregs(r);
    bool ans = true;
    if (cpu.eax != ref_r->eax) ans = false;
    if (cpu.ecx != ref_r->ecx) ans = false;
    if (cpu.edx != ref_r->edx) ans = false;
    if (cpu.ebx != ref_r->ebx) ans = false;
    if (cpu.esp != ref_r->esp) ans = false;
    if (cpu.ebp != ref_r->ebp) ans = false;
    if (cpu.esi != ref_r->esi) ans = false;
    if (cpu.edi != ref_r->edi) ans = false;
    if (cpu.pc != ref_r->pc) ans = false;
    return ans;
}

void isa_difftest_attach(void)
{
}
