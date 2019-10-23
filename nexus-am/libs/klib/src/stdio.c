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

    int zero_padded = 0;
    int in_format = 0;
    unsigned width_now = 0;
    while (*fmt != '\0') {
        if (in_format) {
            switch (*fmt) {
                case 's':  //string
                    s = va_arg(ap, char*);
                    if (strlen(s) >= width_now && width_now) {
                        while (width_now--) *outptr++ = *s++;
                    } else if (zero_padded) {
                        int count0 = width_now - strlen(s);
                        while (count0--) *outptr++ = '0';
                        while (*s != '\0') *outptr++ = *s++;
                    } else {
                        while (*s != '\0') *outptr++ = *s++;
                    }
                    in_format = 0;
                    break;
                case 'd':  //integer
                    d = va_arg(ap, int);
                    if (d < 0) {
                        d = -d;
                        *outptr++ = '-';
                    } else if (d == 0) {
                        *outptr++ = '0';
                    }
                    size_t i;
                    for (i = 1; d; i++, d /= 10)
                        tmpd[i] = (d % 10) + '0';
                    for (i--; i; i--)
                        *outptr++ = tmpd[i];
                    in_format = 0;
                    break;
                case '0':
                    if (*(fmt - 1) == '%')
                        zero_padded = 1;
                    else
                        width_now *= 10;
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    width_now = 10 * width_now + *fmt;
                    break;
                default: break;
            }
            ++fmt;
        } else if (*fmt == '%') {
            in_format = 1;
            width_now = 0;
            zero_padded = 0;
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
    char tmp[1024];
    va_list ap;
    va_start(ap, fmt);
    int ret = vsprintf(tmp, fmt, ap);
    if (ret >= n) {
        strncpy(out, tmp, n - 1);
        *(out + n - 1) = '\0';
        return strlen(tmp);  //ATTENTION!
    } else {
        strcpy(out, tmp);
        return ret;
    }
}

#endif
