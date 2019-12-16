#include "nemu.h"
#include <stdio.h>
paddr_t page_translate(vaddr_t addr);

uint32_t get_DIR(vaddr_t addr)
{
    return addr >> 22;
}
uint32_t get_PAGE(vaddr_t addr)
{
    return (addr >> 12) & 0x3FF;
}
uint32_t get_OFFSET(vaddr_t addr)
{
    return addr & 0xFFF;
}
uint32_t isa_vaddr_read(vaddr_t addr, int len)
{
    if (cpu.cr0.paging) {
        if (get_OFFSET(addr) + len > PAGE_SIZE) {
            int s1 = PAGE_SIZE - get_OFFSET(addr);
            int s2 = len - s1;
            uint32_t t1 = paddr_read(page_translate(addr), s1);
            uint32_t t2 = paddr_read(page_translate(addr + s1), s2);

            return (t2 << (8 * s1)) | t1;
        } else {
            paddr_t paddr = page_translate(addr);
            return paddr_read(paddr, len);
        }
    } else {
        return paddr_read(addr, len);
    }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len)
{
    if (cpu.cr0.paging) {
        if (get_OFFSET(addr) + len > PAGE_SIZE) {
            int s1 = PAGE_SIZE - get_OFFSET(addr);
            int s2 = len - s1;
            paddr_write(page_translate(addr), data << (8 * s2) >> (8 * s2), s1);
            paddr_write(page_translate(addr + s1), data >> (8 * s1), s2);
        } else {
            paddr_t paddr = page_translate(addr);
            paddr_write(paddr, data, len);
        }
    } else {
        paddr_write(addr, data, len);
    }
}

paddr_t page_translate(vaddr_t addr)
{
    PDE pde;
    PTE pte;
    //printf("cr3:%x\n",cpu.cr3.val);
    pde.val = paddr_read((cpu.cr3.page_directory_base << 12) + get_DIR(addr) * sizeof(PDE), sizeof(PDE));
    //printf("vaddr:%x\n", addr);
    assert(pde.present == 1);
    pte.val = paddr_read((pde.page_frame << 12) + get_PAGE(addr) * sizeof(PTE), sizeof(PTE));
    assert(pte.present == 1);
    return (pte.page_frame << 12) + get_OFFSET(addr);
}
