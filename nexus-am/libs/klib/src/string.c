#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len=0;
  for (;s[len] != '\0'; len++);
  return len;
}

char *strcpy(char* dst,const char* src) {
  size_t n=strlen(src),i;
  for (i = 0; i < n; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for ( ; i < n; i++)
    dst[i] = '\0';
  dst[i]='\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t dest_len = strlen(dst);
  size_t i,n=strlen(src);  
  for (i = 0 ; i < n && src[i] != '\0' ; i++)
    dst[dest_len + i] = src[i];
  //dst[dest_len + i] = '\0';

  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i;
  for(i=0;s1[i]==s2[i];++i)
      if(s1[i]=='\0')
        return 0;
  if(s1[i]==s2[i]) return 0;
  return (((unsigned char)s1[i]<(unsigned char)s2[i])?-1:1);
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i;
  for(i=1;*s1==*s2&&i<=n;++s1,++s2)
    if(*s1=='\0')
      return 0;
  if(*s1==*s2) return 0;
  return ((*(unsigned char*)s1<*(unsigned char*)s2)?-1:1);
}

void* memset(void* v,int c,size_t n) {
  size_t i=0;
  char *tar=(char*)v;
  for(;i<n;i++) tar[i]=c;
  //tar[i]='\0';
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  size_t i=0;
  char *dst=(char*)out;char *src=(char*)in;
  for(;i<n;i++){
    *dst=*src;
    dst++;
    src++;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  size_t i;
  char *s3=(char*)s1;char *s4=(char*)s2;
  for(i=1;*s3==*s4&&i<=n;i++,s3++,s4++)
    if(*s3=='\0')
      return 0;
  if((*(unsigned char*)s3!=*(unsigned char*)s4))
    return ((*(unsigned char*)s3<*(unsigned char*)s4)?-1:1);
  return 0;
}

#endif
