#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
#    define Elf_Ehdr Elf64_Ehdr
#    define Elf_Phdr Elf64_Phdr
#else
#    define Elf_Ehdr Elf32_Ehdr
#    define Elf_Phdr Elf32_Phdr
#endif
int printf(const char* fmt, ...);
size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t get_ramdisk_size();
static uintptr_t loader(PCB* pcb, const char* filename)
{
    Elf_Ehdr E_hdr;
    Elf_Phdr P_hdr;
    ramdisk_read(&E_hdr, 0x0, sizeof(Elf_Ehdr));
    printf("%x\n", E_hdr.e_ident);
    for (int i = 0; i < E_hdr.e_phnum; i++) {
        ramdisk_read(&P_hdr, E_hdr.e_phoff + i * E_hdr.e_phentsize, E_hdr.e_phentsize);
        if (P_hdr.p_type == PT_LOAD) {
            uint32_t tmp[0x8FFF];
            ramdisk_read(tmp, P_hdr.p_offset, P_hdr.p_filesz);
            memcpy((void*)P_hdr.p_vaddr, tmp, P_hdr.p_filesz);
            memset((void*)P_hdr.p_filesz, 0, P_hdr.p_memsz - P_hdr.p_filesz);
        }
    }
    return E_hdr.e_entry;
}

void naive_uload(PCB* pcb, const char* filename)
{
    uintptr_t entry = loader(pcb, filename);
    Log("Jump to entry = %x", entry);
    ((void (*)())entry)();
}

void context_kload(PCB* pcb, void* entry)
{
    _Area stack;
    stack.start = pcb->stack;
    stack.end = stack.start + sizeof(pcb->stack);

    pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB* pcb, const char* filename)
{
    uintptr_t entry = loader(pcb, filename);

    _Area stack;
    stack.start = pcb->stack;
    stack.end = stack.start + sizeof(pcb->stack);

    pcb->cp = _ucontext(&pcb->as, stack, stack, (void*)entry, NULL);
}
