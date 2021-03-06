#include "const.h"
#include "global.h"
#include "protect.h"
#include "proto.h"
#include "type.h"

char *itoa(char *str, int num) {
    char *p = str;
    char ch;
    int i;
    int flag = 0;

    *p++ = '0';
    *p++ = 'x';
    if (num == 0) {
        *p++ = '0';
    } else {
        for (i = 28; i >= 0; i -= 4) {
            ch = (num >> i) & 0x0f;
            if (flag || (ch > 0)) {
                flag = 1;
                ch += '0';
                if (ch > '9') {
                    ch += 7;
                }
                *p++ = ch;
            }
        }
    }
    *p = 0;
    return str;
}

char *int2str(char *str, int num) {
    char *p = str;
    int single = 0;
    int rest = num;
    int count = 0;
    do {
        single = rest % 10;
        rest = rest / 10;
        *p = '0' + single;
        p++;
        count++;
    } while (rest != 0);
    *p = '\0';
    p--;
    char temp;
    while (str < p) {
        temp = *str;
        *str = *p;
        *p = temp;
        str++;
        p--;
    }
    return str;
}

void disp_int(int input) {
    char output[16];
    itoa(output, input);
    disp_str(output);
}

unsigned int strlen(char const *string) {
    unsigned int num = 0;
    char const *p = string;
    while (*p) {
        p++;
        num++;
    }
    return num;
}

void *memcpy(void *pDst, void *pSrc, int iSize) {
    u8 *pD = pDst;
    u8 *pS = pSrc;
    for (int i = 0; i < iSize; ++i) {
        *pD = *pS;
        pD++;
        pS++;
    }
}

char *strcpy(char  *p_dst, char const *p_src) {
    char *p_d = p_dst;
    char const *p_s = p_src;
    while (*p_s) {
        *p_d = *p_s;
        p_d++;
        p_s++;
    }
}

void memset(void const *p_dst, char ch, int size) {
    char *p_d = (char *) p_dst;
    for (int i = 0; i < size; ++i) {
        *p_d++ = ch;
    }
}