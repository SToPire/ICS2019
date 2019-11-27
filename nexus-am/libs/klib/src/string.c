#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len=0;
  const char *p=s;
  while(*p!=0){
     len++;
     p++;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  size_t i;
  for(i=0;src[i]!='\0';i++)
    dst[i]=src[i];
  dst[i]='\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for(i=0;i<n&&src[i]!='\0';i++)
    dst[i]=src[i];
  for(;i<n;i++)
    dst[i]='\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t dst_len=strlen(dst);
  size_t i;
  for(i=0;src[i]!='\0';i++)
    dst[dst_len+i]=src[i];
  dst[dst_len+i]='\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  int mark=0;
  const char *cs1=s1;
  const char *cs2=s2;
  for(int i=0;(*cs1!=0) && (*cs2!=0);i++)
  {
    if(*cs1<*cs2)
    {
      mark=-1;
      break;
    }
    if(*cs1>*cs2)
    {
      mark=1;
      break;
    }
    cs1++;
    cs2++;
  }
  if(mark==0)
  {
    if((*cs1==0) && (*cs2!=0)) mark=-1;
    if((*cs1!=0) && (*cs2==0)) mark=1;
  }
  return mark;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  int mark=0;
  const char *cs1=s1;
  const char *cs2=s2;
  size_t n1=n;
  while((*cs1!=0) && (*cs2!=0)&& (n1>0))
  {
    n1--;
    if(*cs1<*cs2)
    {
      mark=-1;
      break;
    }
    if(*cs1>*cs2)
    {
      mark=1;
      break;
    }
    cs1++;
    cs2++;
  }
  if(mark==0)
  {
    if((*cs1==0) && (*cs2!=0)) mark=-1;
    if((*cs1!=0) && (*cs2==0)) mark=1;
  }
  return mark;
}
void* memset(void* v,int c,size_t n) {
  unsigned char *v1=v;
  for(size_t i=0;i<n;i++)
    v1[i]=c;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  unsigned char *v1=out;
  const unsigned char *v2=in;
  for(size_t i=0;i<n;i++)
  v1[i]=v2[i];  
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  int mark=0;
  const unsigned char *cs1=s1;
  const unsigned char *cs2=s2;
  size_t n1=n;
  while((*cs1!=0) && (*cs2!=0) && (n1>0))
  {
    n1--;
    if(*cs1<*cs2)
    {
      mark=-1;
      break;
    }
    if(*cs1>*cs2)
    {
      mark=1;
      break;
    }
    cs1++;
    cs2++;
  }
  return mark;
}

#endif
