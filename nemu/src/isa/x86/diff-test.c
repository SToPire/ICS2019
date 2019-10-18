#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  char r[77];
  ref_difftest_getregs(r);

  return false;
}

void isa_difftest_attach(void) {
}
