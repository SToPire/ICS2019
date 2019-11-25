#include "common.h"
#include "syscall.h"
int fs_open(const char* pathname, int flags, int mode);
int fs_close(int fd);
__ssize_t fs_read(int fd, void* buf, size_t len);
__ssize_t fs_write(int fd, const void* buf, size_t len);
__off_t fs_lseek(int fd, size_t offset, int whence);

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
    return fs_write(fd, buf, count);
}
int sys_brk(intptr_t increment)
{
    return 0;
}

int sys_open(const char* path, int flags, __mode_t mode)
{
    return fs_open(path, flags, mode);
}
int sys_close(int fd)
{
    return fs_close(fd);
}
__ssize_t sys_read(int fd, void* buf, size_t count)
{
    return fs_read(fd, buf, count);
}
__off_t sys_lseek(int fd, __off_t offset, int whence)
{
    return fs_lseek(fd, offset, whence);
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
        case SYS_close: c->GPRx = sys_close((int)a[1]); break;
        case SYS_read: c->GPRx = sys_read((int)a[1], (void*)a[2], (size_t)a[3]); break;
        case SYS_lseek: c->GPRx = sys_lseek((int)a[1], (__off_t)a[2], (int)a[3]); break;
        default: panic("Unhandled syscall ID = %d", a[0]);
    }
    return NULL;
}
