#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int vsprintf(char *out, const char *fmt, va_list ap);

int printf(const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	char s[10000];
  	int ans=vsprintf(s,fmt,arg),i=0;
	while(s[i]!='\0'){
		_putc(s[i]);
		++i;
	}
	return ans;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	char ch;
	char *src;
  	int x,len,num[20],y=0;
	while ((ch = *fmt++)!='\0'){
		if (ch != '%'){
			*out=ch;
      		out++;
      		++y;
			continue;
		}
		switch(*fmt != '\0' ? *fmt++ : '\0'){
			case 'd':
				x=va_arg(ap, int);len=0;
        		if(x<0){
            		*out='-'; ++out;x=-x;
        		}
        		if(x==0) num[++len]=0;
        		while(x){
            		num[++len]=x%10;
            		x/=10;
        		}
        		while(len){
          			*out=num[len--]+48;
          			out++;
        		}
				break;
		  	/*case 'f':
				printf_float(va_arg(arg, float));
			  	break;
		  	case 'c':
			  	putchar(va_arg(arg, char));
				break;*/
		  	case 's':
			    src = va_arg(ap, char *);
				while (*src != '\0'){
					*out=*src++;out++;
      			}		
				break;
		}
	}
  *out='\0';
  return y;
}

int sprintf(char *out, const char *fmt, ...) {
  	va_list arg;
	va_start(arg, fmt);
  	return vsprintf(out,fmt,arg);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
