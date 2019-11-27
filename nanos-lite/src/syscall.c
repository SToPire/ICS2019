#include "common.h"
#include "syscall.h"
#include<stdio.h>
int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
__ssize_t fs_read(int fd, void *buf, size_t len);
__ssize_t fs_write(int fd, void *buf, size_t len);
__off_t fs_lseek(int fd,__off_t offset, int whence);
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case(SYS_exit):Log("sys_exit");_halt(a[1]);c->GPRx=0;break;
    case(SYS_yield):Log("sys_yiled");_yield();c->GPRx=0;break;
    case(SYS_brk):Log("sys_brk");c->GPRx=0;break;
    case(SYS_write):c->GPRx=fs_write(a[1],(void*)a[2],a[3]);break;
    case(SYS_close):Log("sys_close");c->GPRx=fs_close(a[1]);break;
    case(SYS_open):Log("sys_open");c->GPRx=fs_open((const char *)a[1],a[2],a[3]);break;
    case(SYS_read):c->GPRx=fs_read(a[1],(void*)a[2],a[3]);break;
    case(SYS_lseek):Log("sys_lseek");c->GPRx=fs_lseek(a[1],a[2],a[3]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  //printf("%d\n",c->GPRx);
  return NULL;
}
