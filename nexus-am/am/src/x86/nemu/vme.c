#include <am.h>
#include <x86.h>
#include <nemu.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {  // Kernel memory mappings
    {.start = (void*)0, .end = (void*)PMEM_SIZE},
    {.start = (void*)MMIO_BASE, .end = (void*)(MMIO_BASE + MMIO_SIZE)}};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*))
{
    pgalloc_usr = pgalloc_f;
    pgfree_usr = pgfree_f;

    int i;

    // make all PDEs invalid
    for (i = 0; i < NR_PDE; i++) {
        kpdirs[i] = 0;
    }

    PTE* ptab = kptabs;
    for (i = 0; i < NR_KSEG_MAP; i++) {
        uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
        uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
        for (; pdir_idx < pdir_idx_end; pdir_idx++) {
            // fill PDE
            kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

            // fill PTE
            PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
            PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
            for (; pte < pte_end; pte += PGSIZE) {
                *ptab = pte;
                ptab++;
            }
        }
    }

    set_cr3(kpdirs);
    set_cr0(get_cr0() | CR0_PG);
    vme_enable = 1;

    return 0;
}

int _protect(_AddressSpace* as)
{
    PDE* updir = (PDE*)(pgalloc_usr(1));
    as->ptr = updir;
    // map kernel space
    for (int i = 0; i < NR_PDE; i++) {
        updir[i] = kpdirs[i];
    }

    return 0;
}

void _unprotect(_AddressSpace* as)
{
}

static _AddressSpace* cur_as = NULL;
void __am_get_cur_as(_Context* c)
{
    c->as = cur_as;
}

void __am_switch(_Context* c)
{
    if (vme_enable) {
        set_cr3(c->as->ptr);
        cur_as = c->as;
    }
}
#include <stdio.h>
uint32_t get_DIR(void* addr)
{
    return (uint32_t)addr >> 22;
}
uint32_t get_PAGE(void* addr)
{
    return ((uint32_t)addr >> 12) & 0x3FF;
}
uint32_t get_OFFSET(void* addr)
{
    return (uint32_t)addr & 0xFFF;
}
int is_present(uint32_t pg)
{
    return pg & 0x80000000;
}
int _map(_AddressSpace* as, void* va, void* pa, int prot)
{
    //printf("%x %x %x\n", as->ptr,va,pa);
    PDE t_pde = ((PDE*)as->ptr)[get_DIR(va)];
    if (!is_present(t_pde)) {
        t_pde = ((PDE*)as->ptr)[get_DIR(va)] = (uint32_t)pgalloc_usr(1) | 0x80000000;
    }
    printf("%x\n", t_pde);
    return 0;
}

_Context* _ucontext(_AddressSpace* as, _Area ustack, _Area kstack, void* entry, void* args)
{
    uintptr_t* tp = ustack.end - 3 * sizeof(uintptr_t);
    *(tp) = *(tp + 1) = *(tp + 2) = 0;
    _Context* tmp = ustack.end - 3 * sizeof(uintptr_t) - sizeof(_Context);
    tmp->cs = 0x8;
    tmp->eip = (uintptr_t)entry;
    return tmp;
}
