#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list list;
  va_start(list,fmt);
  while(*fmt!=0)
  {
     char c1=*fmt;
     fmt++;
     if(c1=='%')
     {
       char c2=*fmt;
       fmt++;  
       int len1=0;
       switch(c2)
       {
          case('d'):
          {
            char c3[100];
            int a=va_arg(list,int);
            if(a==0)
            {
               c3[len1]='0';
               len1++;
            }
            else
            while(a!=0)
            {
              c3[len1]='0'+a%10;
              a=a/10;
              len1++;
            }
            for(int i=0;i<len1;i++)
            {
              _putc(c3[len1-1-i]);
            }
            break;
          }
          case('s'):
          {
            char *a=va_arg(list,char*);
            while(*a!=0)
            { 
              _putc(*a);
              a++;
            }
            break;
          }
       }
     }
     else  
     {
        _putc(c1);
     }
  }
  va_end(list);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  int len=0;
  va_list list;
  va_start(list,fmt);
  while(*fmt!=0)
  {
     char c1=*fmt;
     fmt++;
     if(c1=='%')
     {
       char c2=*fmt;
       fmt++;  
       int len1=0;
       len++;
       switch(c2)
       {
          case('d'):
          {
            char c3[100];
            int a=va_arg(list,int);
            if(a==0)
            {
               len++;
               c3[len1]='0';
               len1++;
            }
            else
            while(a!=0)
            {
              c3[len1]='0'+a%10;
              a=a/10;
              len1++;
            }
            for(int i=0;i<len1;i++)
            {
              *out=c3[len1-1-i];
              out++;
            }
            break;
          }
          case('s'):
          {
            char *a=va_arg(list,char*);
            while(*a!=0)
            { 
              *out=*a;
              len++;
              out++;
              a++;
            }
            break;
          }
       }
     }
     else  
     {
        *out=c1;
        out++;
     }
  }
  *out='\0';
  va_end(list);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
