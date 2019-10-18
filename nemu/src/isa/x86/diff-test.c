#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  uint32_t r[77];
  ref_difftest_getregs(r);
  bool ans=true;
  if(r[0]!=ref_r->eax) ans=false;
  if(r[1]!=ref_r->ecx) ans=false;
  if(r[2]!=ref_r->edx) ans=false;
  if(r[3]!=ref_r->ebx) ans=false;
  if(r[4]!=ref_r->esp) ans=false;
  if(r[5]!=ref_r->ebp) ans=false;
  if(r[6]!=ref_r->esi) ans=false;
  if(r[7]!=ref_r->edi) ans=false;
  if(r[8]!=ref_r->pc) ans=false;
  printf("%x\n",r[9]);ans=false;
  return ans;
}

void isa_difftest_attach(void) {
}
