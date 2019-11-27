#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);
__ssize_t fs_read(int fd, void *buf, size_t len);
size_t fs_offset(int fd);
int fs_open(const char *pathname, int flags, int mode);
static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr head;Elf_Phdr segment;
 int fd=fs_open(filename,0,0);
  int fileoffset=fs_offset(fd);
  ramdisk_read(&head,fileoffset,sizeof(Elf_Ehdr));
  for(int i=1;i<=head.e_phnum;i++){
    //printf("%d\n",i);
    ramdisk_read(&segment,fileoffset+sizeof(Elf_Ehdr)+(i-1)*sizeof(Elf_Phdr),sizeof(Elf_Phdr));
    if(segment.p_type!=1) continue;
    ramdisk_read((void*)segment.p_vaddr,segment.p_offset+fileoffset,segment.p_filesz);
    void *addr=(void*)segment.p_vaddr+segment.p_filesz;
    memset(addr,0,segment.p_memsz-segment.p_filesz);
  }
  return head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
