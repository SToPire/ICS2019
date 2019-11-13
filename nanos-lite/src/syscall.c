#include "common.h"
#include "syscall.h"

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
    uintptr_t i = 0;
    if (fd == 1 || fd == 2) {
        for (; count > 0; count--) {
            _putc(((char*)buf)[i]);
            i++;
            ;
        }
    }
    return i;
}
_Context* do_syscall(_Context* c)
{
    uintptr_t a[4];
    a[0] = c->GPR1;
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;
    switch (a[0]) {
        case SYS_write: c->GPRx = sys_write((int)a[1], (void*)a[2], (size_t)a[3]); break;
        case SYS_yield: c->GPRx = sys_yield(); break;
        case SYS_exit: c->GPRx = sys_exit((int)a[1]); break;
        default: panic("Unhandled syscall ID = %d", a[0]);
    }

    return NULL;
}
