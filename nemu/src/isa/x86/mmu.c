#include "nemu.h"
#include <stdio.h>
paddr_t page_translate(vaddr_t addr);

uint32_t isa_vaddr_read(vaddr_t addr, int len)
{
    if (cpu.cr0.paging){
        paddr_t paddr = page_translate(addr);
        return paddr_read(paddr, len);
    }else{
        return paddr_read(addr, len);
    }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  paddr_write(addr, data, len);
}

paddr_t page_translate(vaddr_t addr)
{
    PDE t_pde;
    printf("ddd:%p\n",&t_pde);
    printf("dire base:%x\n", cpu.cr3.page_directory_base);
    return addr;
}
