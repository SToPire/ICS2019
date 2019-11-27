#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
   //uint32_t qemu_r[77];
  //ref_difftest_getregs(qemu_r);
  //printf("%x %x %x\n",pc,cpu.pc,ref_r->pc);;
  return (cpu.eax==ref_r->eax)&(cpu.ecx==ref_r->ecx)&(cpu.edx==ref_r->edx)&(cpu.ebx==ref_r->ebx)&(cpu.esp==ref_r->esp)&(cpu.ebp==ref_r->ebp)&(cpu.esi==ref_r->esi)&(cpu.edi==ref_r->edi)&(ref_r->pc==cpu.pc);
}

void isa_difftest_attach(void) {
}
