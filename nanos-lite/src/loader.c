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
extern size_t ramdisk_read(void* buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void* buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_close(int fd);
extern size_t fs_read(int fd, void* buf, size_t len);
extern int fs_open(const char* pathname, int flags, int mode);
extern size_t fs_write(int fd, const void* buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
static uintptr_t loader(PCB* pcb, const char* filename)
{
    Elf_Ehdr elf_header;
    Elf_Phdr pro_header;
    int fd = fs_open(filename, 0, 0);
    fs_read(fd, &elf_header, sizeof(Elf_Ehdr));
    size_t phoffset = elf_header.e_phoff;
    size_t offset = phoffset;
    size_t phnum = elf_header.e_phnum;
    size_t phentsize = elf_header.e_phentsize;
    fs_lseek(fd, offset, SEEK_SET);
    for (int i = 0; i < phnum; i++) {
        fs_read(fd, &pro_header, phentsize);
        if (pro_header.p_type == PT_LOAD) {
            fs_lseek(fd, pro_header.p_offset, SEEK_SET);
            fs_read(fd, (uintptr_t*)pro_header.p_vaddr, pro_header.p_filesz);
            memset((uintptr_t*)(pro_header.p_vaddr + pro_header.p_filesz), 0, pro_header.p_memsz - pro_header.p_filesz);
        }
        offset += phentsize;
        if (i < phnum - 1)
            fs_lseek(fd, offset, SEEK_SET);
    }
    fs_close(fd);
    return elf_header.e_entry;
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
    uintptr_t entry = loader(pcb, filename);

    _Area stack;
    stack.start = pcb->stack;
    stack.end = stack.start + sizeof(pcb->stack);

    pcb->cp = _ucontext(&pcb->as, stack, stack, (void*)entry, NULL);
}
