#include "proc.h"
#include <elf.h>
#include <stdio.h>
#ifdef __ISA_AM_NATIVE__
#    define Elf_Ehdr Elf64_Ehdr
#    define Elf_Phdr Elf64_Phdr
#else
#    define Elf_Ehdr Elf32_Ehdr
#    define Elf_Phdr Elf32_Phdr
#endif

extern int fs_open(const char* pathname, int flags, int mode);
extern int fs_close(int fd);
extern size_t fs_read(int fd, void* buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
size_t get_file_size(int fd);

static uintptr_t loader(PCB* pcb, const char* filename)
{
    int fd = fs_open(filename, 'r', 0);
    Elf_Ehdr E_hdr;
    Elf_Phdr P_hdr;
    fs_read(fd, &E_hdr, sizeof(Elf_Ehdr));

    for (int i = 0; i < E_hdr.e_phnum; i++) {
        fs_lseek(fd, E_hdr.e_phoff + i * E_hdr.e_phentsize, SEEK_SET);
        fs_read(fd, &P_hdr, E_hdr.e_phentsize);
        if (P_hdr.p_type == PT_LOAD) {
            fs_lseek(fd, P_hdr.p_offset, SEEK_SET);
            void* vaddr = (void*)P_hdr.p_vaddr;
            uint32_t i = 0;
            for (; i < P_hdr.p_filesz; i += PGSIZE) {
                void* paddr = new_page(1);
                uint32_t sz = (P_hdr.p_filesz - i >= PGSIZE) ? PGSIZE : (P_hdr.p_filesz - i);
                _map(&pcb->as, vaddr + i, paddr, 0);
                fs_read(fd, paddr, sz);
            }
            vaddr = vaddr + i;
            i = 0;
            uint32_t zero_size = P_hdr.p_memsz - P_hdr.p_filesz;
            for (; i < zero_size;i += PGSIZE){
                void* paddr = new_page(1);
                uint32_t sz = (zero_size - i >= PGSIZE) ? PGSIZE : (zero_size - i);
                _map(&pcb->as, vaddr + i, paddr, 0);
                memset(paddr, 0, sz);
            }
            pcb->max_brk = PGROUNDUP((uint32_t)vaddr + i);
        }
    }
    fs_close(fd);
    return E_hdr.e_entry;
}

void naive_uload(PCB* pcb, const char* filename)
{
    uintptr_t entry;
    entry = loader(pcb, filename);
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
    _protect(&pcb->as);
    uintptr_t entry = loader(pcb, filename);
    _Area stack;
    stack.start = pcb->stack;
    stack.end = stack.start + sizeof(pcb->stack);

    pcb->cp = _ucontext(&pcb->as, stack, stack, (void*)entry, NULL);
}
