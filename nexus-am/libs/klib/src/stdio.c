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
    /* version 2.0 on Oct 23th, 2019, width and zero flag supported*/
    char* outptr = out;
    char* s;
    int d;
    unsigned u;
    char tmp_num[20];

    int d_negative_no_zero_padded_flag = 0;  //negative number with no zero_padded

    int zero_padded = 0;
    int in_format = 0;
    int width_now = 0;
    size_t i;
    while (*fmt != '\0') {
        if (in_format) {
            switch (*fmt) {
                case 's':  //string
                    s = va_arg(ap, char*);
                    int count_s = width_now - strlen(s);
                    while (count_s-- > 0) {
                        if (zero_padded)
                            *outptr++ = '0';
                        else
                            *outptr++ = ' ';
                    }
                    while (*s != '\0') *outptr++ = *s++;
                    in_format = 0;
                    break;
                case 'd':  //integer
                    d = va_arg(ap, int);
                    if (d < 0) {
                        d = -d;
                        if (zero_padded) {
                            *outptr++ = '-';
                            --width_now;
                        } else
                            d_negative_no_zero_padded_flag = 1;
                    } else if (d == 0) {
                        *outptr++ = '0';
                        --width_now;
                        while (width_now-- > 0) *outptr++ = '0';
                    }
                    for (i = 1; d; i++, d /= 10)
                        tmp_num[i] = (d % 10) + '0';
                    if (d_negative_no_zero_padded_flag)
                        tmp_num[i++] = '-';
                    int count_d = width_now - (i - 1);
                    while (count_d-- > 0) {
                        if (zero_padded)
                            *outptr++ = '0';
                        else
                            *outptr++ = ' ';
                    }
                    for (i--; i; i--)
                        *outptr++ = tmp_num[i];
                    in_format = 0;
                    break;
                case 'u':  //unsigned
                    u = va_arg(ap, unsigned);
                    if (u == 0) {
                        *outptr++ = '0';
                        --width_now;
                        while (width_now-- > 0) *outptr++ = '0';
                    }
                    for (i = 1; u; i++, u /= 10)
                        tmp_num[i] = (u % 10) + '0';
                    int count_u = width_now - (i - 1);
                    while (count_u-- > 0) {
                        if (zero_padded)
                            *outptr++ = '0';
                        else
                            *outptr++ = ' ';
                    }
                    for (i--; i; i--)
                        *outptr++ = tmp_num[i];
                    in_format = 0;
                    break;
                case 'x':  //hex
                    u = va_arg(ap, unsigned);
                    if (u == 0) {
                        *outptr++ = '0';
                        --width_now;
                        while (width_now-- > 0) *outptr++ = '0';
                    }
                    for (i = 1; u; i++, u /= 16) {
                        if (u % 16 <= 9) {
                            tmp_num[i] = (u % 16) + '0';
                        } else {
                            tmp_num[i] = (u % 16) + 'a' - 10;
                        }
                    }
                    int count_x = width_now - (i - 1);
                    while (count_x-- > 0) {
                        if (zero_padded)
                            *outptr++ = '0';
                        else
                            *outptr++ = ' ';
                    }
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
                    width_now = 10 * width_now + *fmt - '0';
                    break;
                default: assert(0);
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
