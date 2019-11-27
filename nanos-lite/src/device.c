#include "common.h"
#include <amdev.h>
size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *st=(char*)buf;
  for(int i=0;i<len;i++)
  _putc(st[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
 // Log("now come to events_read");
  int keynow=read_key();
  char temp[256];
  if(keynow!=0)
  {
    if((keynow&0x8000)!=0)
    {
      keynow=keynow-0x8000;
      sprintf(temp,"kd %s\n",keyname[keynow]);
     // Log("%s",temp);
    }
    else
    {
      sprintf(temp,"ku %s\n",keyname[keynow]);
    //  Log("%s",temp);
    }
    strncpy(buf,temp,len);
  }
  else  
  {
    uint32_t nowtime=uptime();
    sprintf(temp,"t %d\n",nowtime);
    strncpy(buf,temp,len);
    //Log("%s",temp);
  }
  if(strlen(temp)<len) len=strlen(temp);
  return len;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
