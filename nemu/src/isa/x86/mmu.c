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

uint32_t get_DIR(vaddr_t addr)
{
    return addr >> 22;
}
paddr_t page_translate(vaddr_t addr)
{
    PDE pde;
    //PTE pte;
    pde.val = paddr_read((cpu.cr3.page_directory_base << 12) + get_DIR(addr) * sizeof(PDE), sizeof(PDE));
    assert(pde.present == 1);
    return addr;
}
