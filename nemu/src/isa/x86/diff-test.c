#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  char r[77];
  ref_difftest_setregs(r);
  printf("%s\n",r);
  return true;
}

void isa_difftest_attach(void) {
}
