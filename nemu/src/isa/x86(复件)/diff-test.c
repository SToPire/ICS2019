#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool df=false;
  if((ref_r->eax==cpu.eax) && (ref_r->ecx==cpu.ecx) && (ref_r->ebx==cpu.ebx) && 
  (ref_r->esp==cpu.esp) && (ref_r->edx==cpu.edx) && (ref_r->ebp==cpu.ebp) && 
  (ref_r->edi==cpu.edi) && (ref_r->esi==cpu.esi)) df=true;
  return df;
}

void isa_difftest_attach(void) {
}
