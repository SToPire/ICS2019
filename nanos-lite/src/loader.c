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
size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t get_file_disk_offset(int fd);
size_t get_file_size(int fd);

int fs_open(const char* pathname, int flags, int mode);
int fs_close(int fd);
__ssize_t fs_read(int fd, void* buf, size_t len);
__off_t fs_lseek(int fd, size_t offset, int whence);

static uintptr_t loader(PCB* pcb, const char* filename)
{
    int fd = fs_open(filename, 'r', 0);
    //int file_offset = get_file_disk_offset(fd);
    int file_sz = get_file_size(fd);
    Elf_Ehdr E_hdr;
    Elf_Phdr P_hdr;
    //ramdisk_read(&E_hdr, file_offset, sizeof(Elf_Ehdr));
    fs_read(fd, &E_hdr, sizeof(Elf_Ehdr));

    for (int i = 0; i < E_hdr.e_phnum; i++) {
        fs_lseek(fd, E_hdr.e_phoff + i * E_hdr.e_phentsize, SEEK_SET);
        fs_read(fd, &P_hdr, E_hdr.e_phentsize);
        // ramdisk_read(&P_hdr, file_offset + E_hdr.e_phoff + i * E_hdr.e_phentsize, E_hdr.e_phentsize);
        if (P_hdr.p_type == PT_LOAD) {
            uint32_t tmp[file_sz];
            fs_lseek(fd, P_hdr.p_offset, SEEK_SET);
            fs_read(fd, tmp, P_hdr.p_filesz);
            //ramdisk_read(tmp, file_offset + P_hdr.p_offset, P_hdr.p_filesz);
            memcpy((void*)P_hdr.p_vaddr, tmp, P_hdr.p_filesz);
            memset((void*)(P_hdr.p_vaddr + P_hdr.p_filesz), 0, P_hdr.p_memsz - P_hdr.p_filesz);
        }
    }
    fs_close(fd);
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
