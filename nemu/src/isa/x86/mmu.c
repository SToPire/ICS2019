#include "nemu.h"
#include <stdio.h>
uint32_t isa_vaddr_read(vaddr_t addr, int len) {
    if (cpu.CR0.PG) printf("cr3:%x\n",cpu.cr3);
    return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  paddr_write(addr, data, len);
}
