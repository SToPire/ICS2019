#include "common.h"
#include "syscall.h"
int fs_open(const char* pathname, int flags, int mode);
int sys_yield()
{
    _yield();
    return 0;
}
int sys_exit(int status)
{
    _halt(status);
    return 0;
}
int sys_write(int fd, void* buf, size_t count)
{
    if (fd == 1 || fd == 2) {
        for (int i = 0; i < count; i++)
            _putc(*(char*)(buf + i));
        return count;
    } else
        return 0;
}
int sys_brk(intptr_t increment)
{
    return 0;
}

int sys_open(const char* path, int flags, __mode_t mode)
{
    //return fs_open(path, flags, mode);
    return 0;
}
_Context* do_syscall(_Context* c)
{
    uintptr_t a[4];
    a[0] = c->GPR1;
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;
    switch (a[0]) {
        case SYS_exit: c->GPRx = sys_exit((int)a[1]); break;
        case SYS_yield: c->GPRx = sys_yield(); break;
        case SYS_write: c->GPRx = sys_write((int)a[1], (void*)a[2], (size_t)a[3]); break;
        case SYS_brk: c->GPRx = sys_brk((intptr_t)a[1]); break;
        case SYS_open: c->GPRx = sys_open((const char*)a[1], (int)a[2], (__mode_t)a[3]); break;
        default: panic("Unhandled syscall ID = %d", a[0]);
    }
    return NULL;
}
