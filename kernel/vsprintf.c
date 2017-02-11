//
// Created by xiazihao on 2/10/17.
//
#include "type.h"
#include "proto.h"

int vsprintf(char *buf, const char *format, va_list args) {
    char *p;
    char temp[256];
    char *s;
    va_list p_next_arg = args;
    for (p = buf; *format; format++) {
        if (*format != '%') {
            *p++ = *format;
            continue;
        }
        format++;
        switch (*format) {
            case 'x':
                itoa(temp, *((int *) p_next_arg));
                strcpy(p, temp);
                p_next_arg += 4;
                p += strlen(temp);
                break;
            case 's':
                s = *(char **) p_next_arg;
                while (*s) {
                    *p = *s;
                    p++;
                    s++;
                }
                p_next_arg += 4;
                break;
            case 'd':
                break;
            default:
                break;
        }
    }
    return (p - buf);
}