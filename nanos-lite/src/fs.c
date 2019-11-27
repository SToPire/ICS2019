#include "fs.h"

size_t serial_write(const void* buf, size_t offset, size_t len);
size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(const void* buf, size_t offset, size_t len);
size_t events_read(void* buf, size_t offset, size_t len);
size_t fbsync_write(const void* buf, size_t offset, size_t len);
size_t dispinfo_read(void* buf, size_t offset, size_t len);
size_t fb_write(const void* buf, size_t offset, size_t len);
int screen_width();
int screen_height();

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
    {"/dev/fb", 0, 0, invalid_read, fb_write},
    {"/dev/events", 0, 0, events_read, invalid_write},
    {"/dev/fbsync", 0, 0, invalid_read, fbsync_write},
    {"/proc/dispinfo", 128, 0, dispinfo_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs()
{
    file_table[FD_FB].size = screen_height() * screen_width() * 4;
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
    printf("open:%s %d %d ", pathname, flags, mode);

    for (int i = 0; i < NR_FILES; i++) {
        printf("filetable[%d]:%s\n", i, file_table[i].name);
        if (strcmp(file_table[i].name, pathname) == 0) {
            file_table[i].open_offset = 0;
            printf("%d\n", i);
            return i;
        }
    }
    assert(0);
}

int fs_close(int fd)
{
    return 0;
}

__ssize_t fs_read(int fd, void* buf, size_t len)
{
    printf("read\n");

    Finfo* cur_file = &file_table[fd];
    if (!cur_file->read) {
        if (cur_file->open_offset > cur_file->size) return 0;
        if (len > cur_file->size - cur_file->open_offset)
            len = cur_file->size - cur_file->open_offset;
        ramdisk_read(buf, cur_file->disk_offset + cur_file->open_offset, len);
    } else
        len = cur_file->read(buf, cur_file->disk_offset + cur_file->open_offset, len);
    cur_file->open_offset += len;
    return len;
}

__ssize_t fs_write(int fd, const void* buf, size_t len)
{
    printf("write\n");

    Finfo* cur_file = &file_table[fd];
    if (!cur_file->write) {
        if (cur_file->open_offset > cur_file->size) return 0;
        if (len > cur_file->size - cur_file->open_offset)
            len = cur_file->size - cur_file->open_offset;
        ramdisk_write(buf, cur_file->disk_offset + cur_file->open_offset, len);
    } else
        len = cur_file->write(buf, cur_file->disk_offset + cur_file->open_offset, len);
    cur_file->open_offset += len;
    return len;
}

__off_t fs_lseek(int fd, size_t offset, int whence)
{
    printf("lseek\n");

    Finfo* cur_file = &file_table[fd];
    switch (whence) {
        case SEEK_SET:
            if (offset >= 0 && offset <= cur_file->size)
                cur_file->open_offset = offset;
            else
                return -1;
            break;
        case SEEK_CUR:
            if (cur_file->open_offset + offset >= 0 && cur_file->open_offset + offset <= cur_file->size)
                cur_file->open_offset += offset;
            else
                return -1;
            break;
        case SEEK_END:
            cur_file->open_offset = cur_file->size + offset;
            break;
        default: return (__off_t)(-1);
    }
    return cur_file->open_offset;
}