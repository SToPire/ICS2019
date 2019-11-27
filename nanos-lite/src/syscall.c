#include "common.h"
#include "syscall.h"
extern char _end;
typedef size_t ssize_t;
typedef size_t off_t;
extern int fs_open(const char *pathname,int flags,int mode);
extern ssize_t fs_read(int fd,void *buf,size_t len);
extern off_t fs_lseek(int fd,size_t offset,int whence);
extern int fs_close(int fd);
extern ssize_t fs_write(int fd,const void *buf,size_t len);
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_exit : 
    {
      _halt(a[1]); break;
    }
    case SYS_yield :
    {
      _yield(); 
      c->GPRx=0;
      break;
    }
    case SYS_lseek :   
    {
      c->GPRx=(off_t)fs_lseek((int)a[1],(off_t)a[2],(int)a[3]);break;
      break;
    }   
    case SYS_open :
    {
      c->GPRx=(int)fs_open((const char*)a[1],(int)a[2],(size_t)a[3]);break;
      break;
    }
    case SYS_read :
    {
      c->GPRx=(ssize_t)fs_read((int)a[1],(void *)a[2],(size_t)a[3]);break;
      break;
    }
    case SYS_write :
    {
      c->GPRx=(ssize_t)fs_write((int)a[1],(const void *)a[2],(size_t)a[3]);break;
      break;
    }
    case SYS_close :
    {
      c->GPRx=(int)fs_close((int)a[1]);
      break;
    }   
    case SYS_brk :
    {
      if(a[1]<0x20000000)
      {
        _end=*((char*)a[1]);
        c->GPRx=0;
      }
      else c->GPRx=-1;
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
