#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  printf("%x\n",ref_r->eax);
  uint32_t r[77];
  ref_difftest_getregs(r);
  for(int i=0;i<77;i++)
     printf("%u ",r[i]);
  return false;
}

void isa_difftest_attach(void) {
}
