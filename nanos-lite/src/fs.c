#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  __off_t disk_offset;
  __off_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len) ;
enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT,FD_FB,FD_FBSYNC,FD_DISPINFO};
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
size_t events_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0,invalid_read, invalid_write},
  {"stdout", 0, 0, 0,invalid_read, serial_write},
  {"stderr", 0, 0, 0,invalid_read, serial_write},
  [FD_EVENT]={"/dev/events", 0, 0,0,events_read, invalid_write},
  [FD_FB]={"/dev/fb",0,0,0,invalid_read,fb_write},
  [FD_FBSYNC]={"/dev/fbsync",0,0,0,invalid_read,fbsync_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0,0,dispinfo_read,invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size=400*300*8;
}
int fs_close(int fd) {
    return 0;
}
int fs_open(const char *pathname, int flags, int mode){
    for(int i=0;i<NR_FILES;++i){ 
      //printf("%d %s %s\n",i,file_table[i].name,pathname);
      if(strcmp(pathname,file_table[i].name)==0)
        return i;
    }
    assert(0);
}
__ssize_t fs_read(int fd, void *buf, size_t len){
    if(file_table[fd].write==NULL){
      Log("file_read");
      if (file_table[fd].open_offset >= file_table[fd].size) return 0;
		  if (file_table[fd].open_offset + len > file_table[fd].size)
				len = file_table[fd].size - file_table[fd].open_offset;
		  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		  file_table[fd].open_offset += len;	
      return len;
    }
    else{
      len=file_table[fd].read(buf,file_table[fd].open_offset,len);
      file_table[fd].open_offset += len;	
      return len;
    }
}
__ssize_t fs_write(int fd, void *buf, size_t len){
    if(file_table[fd].write==NULL){
      Log("file_write");
      if (file_table[fd].open_offset >= file_table[fd].size) return 0;
		  if (file_table[fd].open_offset + len > file_table[fd].size)
				len=file_table[fd].size-file_table[fd].open_offset;
      ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		  file_table[fd].open_offset += len;
      
      return len;
    }
    else{
        len=file_table[fd].write(buf,file_table[fd].open_offset,len);
        file_table[fd].open_offset += len;	//printf("%d\n",len);
        return len;
    }
    
}
size_t fs_offset(int fd){
  return file_table[fd].disk_offset;
}
__off_t fs_lseek(int fd,__off_t offset, int whence){
  switch(whence){
    case(SEEK_SET):file_table[fd].open_offset=offset;break;
    case(SEEK_CUR):file_table[fd].open_offset+=offset;break;
    case(SEEK_END):file_table[fd].open_offset=file_table[fd].size+offset;break;
    default: return -1;
  }
  //rintf("__%d %d %d\n",offset,whence,file_table[fd].open_offset);
  return file_table[fd].open_offset;
}