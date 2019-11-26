#include "fs.h"

size_t serial_write(const void* buf, size_t offset, size_t len);
size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(const void* buf, size_t offset, size_t len);
size_t events_read(void* buf, size_t offset, size_t len);

typedef size_t (*ReadFn)(void* buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void* buf, size_t offset, size_t len);

typedef struct {
    char* name;
    size_t size;
    size_t disk_offset;
    ReadFn read;
    WriteFn write;
    size_t open_offset;
} Finfo;

enum { FD_STDIN,
       FD_STDOUT,
       FD_STDERR,
       FD_FB };

size_t invalid_read(void* buf, size_t offset, size_t len)
{
    panic("should not reach here");
    return 0;
}

size_t invalid_write(const void* buf, size_t offset, size_t len)
{
    panic("should not reach here");
    return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    {"stdin", 0, 0, invalid_read, invalid_write},
    {"stdout", 0, 0, invalid_read, serial_write},
    {"stderr", 0, 0, invalid_read, serial_write},
    {"/dev/events", 110, 0, events_read, NULL},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs()
{
    // TODO: initialize the size of /dev/fb
}
size_t get_file_size(int fd)
{
    return file_table[fd].size;
}
size_t get_file_disk_offset(int fd)
{
    return file_table[fd].disk_offset;
}
int fs_open(const char* pathname, int flags, int mode)
{
    for (int i = 0; i < NR_FILES; i++)
        if (strcmp(file_table[i].name, pathname) == 0)
            return i;
    assert(0);
}

int fs_close(int fd)
{
    return 0;
}

__ssize_t fs_read(int fd, void* buf, size_t len)
{
    Finfo* cur_file = &file_table[fd];
    if (len > cur_file->size - cur_file->open_offset)
        len = cur_file->size - cur_file->open_offset;
    ramdisk_read(buf, cur_file->disk_offset + cur_file->open_offset, len);
    cur_file->open_offset += len;
    return len;
}

__ssize_t fs_write(int fd, const void* buf, size_t len)
{
    Finfo* cur_file = &file_table[fd];
    if (!cur_file->write) {
        if (len > cur_file->size - cur_file->open_offset)
            len = cur_file->size - cur_file->open_offset;
        ramdisk_write(buf, cur_file->disk_offset + cur_file->open_offset, len);
    } else
        cur_file->write(buf, cur_file->disk_offset + cur_file->open_offset, len);
    cur_file->open_offset += len;
    return len;
}

__off_t fs_lseek(int fd, size_t offset, int whence)
{
    Finfo* cur_file = &file_table[fd];
    switch (whence) {
        case SEEK_SET:
            cur_file->open_offset = offset;
            break;
        case SEEK_CUR:
            cur_file->open_offset += offset;
            break;
        case SEEK_END:
            cur_file->open_offset = cur_file->size + offset;
            break;
        default: return -1;
    }
    return cur_file->open_offset;
}