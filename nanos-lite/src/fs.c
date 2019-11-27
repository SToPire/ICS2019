#include "fs.h"

extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
typedef size_t ssize_t;
typedef size_t off_t;
typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

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
  {"stdin", 0, 0, 0,invalid_read, invalid_write},
  {"stdout", 0, 0, 0,invalid_read, serial_write},
  {"stderr", 0, 0, 0,invalid_read, serial_write},
  {"/dev/fb",0, 0, 0,invalid_read, invalid_write},
  {"/dev/events",0, 0, 0,events_read,invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  int width=screen_width();
  int height=screen_height();
  file_table[FD_FB].size=width*height*4;
  // 看起来是一个W * H * 4字节的数组，按行优先存储所有像素的颜色值(32位)。每个像素是00rrggbb的形式，8位颜色。屏幕大小从`/proc/dispinfo`文件中获得。

  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname,int flags,int mode)
{
   int mark=0;
   for(int i=0;i<NR_FILES;i++)
   {
      if(strcmp(file_table[i].name,pathname)==0)
      {
        mark=i;
        file_table[i].open_offset=0;
        Log("I have open the file %d",mark);
        return mark;
      }
   }
   panic("can't find this file");
   return -1;
}
ssize_t fs_read(int fd,void *buf,size_t len)
{
  size_t rlen=len;
  if(!file_table[fd].read)
  {
    if(file_table[fd].open_offset+len>=file_table[fd].size)
    {
     rlen=file_table[fd].size-file_table[fd].open_offset;
     Log("read too much information");
    }
    ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,rlen); 
  }
  else
  rlen=file_table[fd].read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,rlen);
  file_table[fd].open_offset+=rlen;
 // Log("I have read the file %d with len %d",fd,rlen);
  return rlen;
}
int fs_close(int fd)
{
  Log("I have closed the file %d",fd);
  return 0;
}
ssize_t fs_write(int fd,const void *buf,size_t len)
{
  size_t rlen;
  if(!file_table[fd].write)
  {
    if(len+file_table[fd].open_offset>file_table[fd].size)
    {
       rlen=file_table[fd].size-file_table[fd].open_offset;
    }
    else rlen=len;
    ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset,rlen);
  }
  else
  rlen = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  return rlen;
}
off_t fs_lseek(int fd,off_t offset,int whence)
{
   Log("I have change the offset %d with len %d",fd,offset);
   switch(whence)
   {
     case SEEK_SET: 
     {
        file_table[fd].open_offset=offset;
        break;
     }
     case SEEK_CUR: 
     {
        file_table[fd].open_offset+=offset;
        break;
     }
     case SEEK_END: 
     {
        file_table[fd].open_offset=file_table[fd].size+offset;
        break;
     }
     default:return -1;
   }
   if(file_table[fd].open_offset>file_table[fd].size)
   file_table[fd].open_offset=file_table[fd].size;
   return file_table[fd].open_offset;
}
  

