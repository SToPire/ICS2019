#include "fs.h"

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern int screen_width();
extern int screen_height();
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t fbsync_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_dispinfo};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/fbsync", 0, 0 ,0, invalid_read, fbsync_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  
  file_table[FD_FB].size = screen_width() * screen_height()*4;
}

int is_str_equal(const char *a, const char *b) {
  int l = strlen(a);
  if (l != strlen(b)) return 0;
  for (int i=0;i<l;i++) {
    if (a[i] != b[i]) return 0;
  }
  return 1;
}

int fs_open(const char *pathname, int flags, int mode) {
  // printf("file name: %s\n", pathname);
  for (int i=0;i<NR_FILES;i++) {
    if (is_str_equal(file_table[i].name, pathname)) {
      file_table[i].open_offset=0;
      return i;
    }
  }
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {
  // assert(fd>=0 && fd<NR_FILES);
  if (!(fd>=0 && fd<NR_FILES)) return -1;

  if (file_table[fd].read != NULL) {
    len =  file_table[fd].read(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }

  if (file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  
  if (len<=0 || fd<FD_FB) //stdin, stdout, stderr
    return 0;

  ramdisk_read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  // assert(fd>=0 && fd<NR_FILES);
  if (!(fd>=0 && fd<NR_FILES)) return -1;

  if (file_table[fd].write != NULL) { //have a write function
  // printf("write fd=%d\n",fd);
    len = file_table[fd].write(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }

  if (file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  
  if (len<=0)
    return 0;
  
  ramdisk_write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
  file_table[fd].open_offset += len;
  return len;
}



size_t fs_lseek(int fd, size_t offset, int whence) {
  // assert(fd>=0 && fd<NR_FILES);
  // printf("fd = %d Call lseek! offset = %d whence = %d\n",fd,offset,whence);
  if (!(fd>=0 && fd<NR_FILES)) return -1;

  if (fd<FD_FB) return 0; //stdin, stdout, stderr

  switch (whence)
  {
    case SEEK_SET: {
      if (offset<0) return -1; //error
      file_table[fd].open_offset = offset; 
    } break;
    case SEEK_CUR: file_table[fd].open_offset += offset; break;
    case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
    default: assert(0);
  }

  //TODO: ???
  if (file_table[fd].open_offset<0) file_table[fd].open_offset=0;
  if (file_table[fd].open_offset>file_table[fd].size) file_table[fd].open_offset=file_table[fd].size;

  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  return 0;
}

// size_t fs_filesz(int fd) {
//   return file_table[fd].size;
// }
