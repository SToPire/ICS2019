#include "common.h"
#include "syscall.h"

int sys_yield()
{
    _yield();
    return 0;
}
int sys_exit(uintptr_t arg)
{
    _halt(arg);
    return 0;
}
int sys_write(uintptr_t fd, uintptr_t buf, uintptr_t count)
{
    if (fd == 1 || fd == 2) {
        for (int i = 0; i < count; i++)
            _putc(*(char*)(buf + i));
        return count;
    }
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
        case SYS_write: c->GPRx = sys_write(a[1], a[2], a[3]); break;
        case SYS_yield: c->GPRx = sys_yield(); break;
        case SYS_exit: c->GPRx = sys_exit(a[1]); break;
        default: panic("Unhandled syscall ID = %d", a[0]);
    }

    return NULL;
}
