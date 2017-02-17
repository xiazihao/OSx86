//
// Created by xiazihao on 2/10/17.
//
#include "type.h"
#include "proto.h"

/**
 * Print string to screen
 * @param format
 * @param ...
 * @return return the size of printed string
 */
int printf(const char *format, ...) {
    int i;
    char buf[256];
    va_list arg = (va_list) ((char *) (&format) + 4);
    i = vsprintf(buf, format, arg);
    write(buf, i);
    return i;
}