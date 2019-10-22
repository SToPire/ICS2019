#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char out[1024];
    int cnt = vsprintf(out, fmt, ap);
    for (char* p = out; *p; p++) _putc(*p);
    return cnt;

    va_end(ap);
}

int vsprintf(char* out, const char* fmt, va_list ap)
{
    /* version 1.0 on Oct 18th, 2019, only %s and %d supported*/
    char* outptr = out;
    char* s;
    int d;
    char tmpd[20];

    while (*fmt != '\0') {
        if (*fmt == '%') {
            switch (*(++fmt)) {
                case 's':  //string
                    s = va_arg(ap, char*);
                    while (*s != '\0')
                        *outptr++ = *s++;
                    break;
                case 'd':  //integer
                    d = va_arg(ap, int);
                    if (d < 0) {
                        d         = -d;
                        *outptr++ = '-';
                    } else if (d == 0) {
                        *outptr++ = '0';
                    }
                    size_t i;
                    for (i = 1; d; i++, d /= 10)
                        tmpd[i] = (d % 10) + '0';
                    for (i--; i; i--)
                        *outptr++ = tmpd[i];
                    break;
            }
            ++fmt;
        } else {
            *outptr = *fmt;
            ++fmt;
            ++outptr;
        }
    }
    *outptr = '\0';
    return outptr - out;
}

int sprintf(char* out, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int tmp = vsprintf(out, fmt, ap);
    va_end(ap);
    return tmp;
}

int snprintf(char* out, size_t n, const char* fmt, ...)
{
    // char tmp[1024];
    // va_list ap;
    // va_start(ap, fmt);
    // if (vsprintf(tmp, fmt, ap) >= n) {
    //     strncpy(out, tmp, n - 1);
    //     *(out + n - 1) = '\0';
    //     return n - 1;
    // }
    return 0;
}

#endif
