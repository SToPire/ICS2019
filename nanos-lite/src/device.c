#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t num=0;
  for(int i=0;i<len;++i){
      char *s=(char *)(buf+i);
      if(*s!='\0') _putc(*s);
      else break;
      ++num;
  }
  return num;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};
int read_key();
uint32_t uptime();
size_t events_read(void *buf, size_t offset, size_t len) {
  int key=read_key(),t=uptime();
  char s[101];
  if(key==0){
    int len1=(t!=0)?2:3;
    s[0]='t';s[1]=' ';s[2]='0';
    while(t){
      s[len1++]=t%10+48;
      t/=10;
    }
    s[len1++]='\n';
    if(len1<len) len=len1;
    memcpy(buf,s,len);
    return len;
  }
  else{
    int len1=0;
    if(key&0x8000){
      s[len1++]='k';s[len1++]='u';s[len1++]=' ';
    }
    else{
      s[len1++]='k';s[len1++]='d';s[len1++]=' ';
    }
    memcpy(s+len1,keyname[key&0xff],strlen(keyname[key&0xff]));
    len1+=strlen(keyname[key&0xff]);
    s[len1++]='\n';
    if(len1<len) len=len1;
    memcpy(buf,s,len);
    return len;
  }
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
   Log("dispinfo_read");
   if(offset+len>128)
    len=128-offset;
  memcpy(buf,dispinfo+offset,len);
  return len;
}
void draw_rect(uint32_t *pixels, int x, int y, int w, int h);
int screen_width(); 
int screen_height();
size_t fb_write(const void *buf, size_t offset, size_t len) {
 // draw_rect((unit32_t *)buf,,,);
 //printf("yes\n");
 
  Log("fb_write");
  if((offset+len)/4>screen_width()*screen_height())
    len=screen_width()*screen_height()*4-offset;
  //printf("%d %d %d\n",(offset+len)/4,screen_width(),screen_height());
  int width=screen_width();
  int x=(offset/4)%width,y=(offset/4)/width;
  int now=(x+len/4<=width)?len/4:width-x;
  draw_rect((uint32_t *)buf,x,y,now,1);
  if(now<len/4&&len/4-now>width){
    draw_rect((uint32_t *)buf+now,0,y+1,width,(len/4-now)/width);
    now+=(len/4-now)/width*width;
    y+=(len/4-now)/width+1;
  }
  if(now<len/4)  draw_rect((uint32_t *)buf+now,0,y+1,width,(len/4-now)/width);
  return len;
}

void draw_sync() ;
size_t fbsync_write(const void *buf, size_t offset, size_t len) { 
  Log("fbsync_write");
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d",screen_width(),screen_height());
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
