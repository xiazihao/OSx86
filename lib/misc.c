//
// Created by xiazihao on 2/10/17.
//

#include <proto.h>

void assertion_failure(char const *exp, char const *file, char const *base_file, const int line, char const *function) {
    printf("\nassert(%s) failed in file: %s\nbase file: %s\nline: %d\nfunction: %s\n", exp, file, base_file, line,
           function);
    while (1);
    __asm__ __volatile__("ud2");
}