//
// Created by xiazihao on 2/14/17.
//

#ifndef CHP6_LIB_H
#define CHP6_LIB_H

//klib.c
void *memcpy(void *pDst, void *pSrc, int iSize);

void memset(void const *p_dst, char ch, int size);

char *strcpy(const char *p_dst, const char *p_src);

char *itoa(char *str, int num);

char *int2str(char *str, int num);

unsigned int strlen(const char *string);

void assertion_failure(char *exp, char *file, char *base_file, int line, char *function);

#define assert(exp) if(exp);else assertion_failure(#exp,__FILE__,__BASE_FILE__,__LINE__,__FUNCTION__)
#endif //CHP6_LIB_H
