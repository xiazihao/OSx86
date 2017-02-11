#ifndef __proto_H
#define __proto_H

#include "const.h"
#include "type.h"
#include "process.h"

#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

//system call
#define INT_VECTOR_SYS_CALL 0x90

//kliba.asm
public void out_byte(u16 port, u8 value);

public u8 in_byte(u16 port);

public void disp_str(char *info);

public void disp_color_str(char *info, int color);

public void disp_int(int input);
//i8259.h
public void init_8259A();

public void put_irq_handler(int irq, irq_handler handler);
//klib.c
public void *memcpy(void *pDst, void *pSrc, int iSize);

public void memset(void const *p_dst, char ch, int size);

public char *strcpy(const char *p_dst, const char *p_src);

char *itoa(char *str, int num);

unsigned int strlen(const char *string);
//kernel.asm
public void restart();

public void disable_irq(int irq);

public void enable_irq(int irq);

public void disable_int();

public void enable_int();
//clock.c
public void milli_delay(int milli_sec);

//vsprintf.c
int vsprintf(char *buf, const char *format, va_list args);

//printf.c
int printf(const char *format, ...);

//tty.c
int sys_write(PROCESS *p_process,char *buf, int len);

//syscall.asm
void write(char *buf, int len);

#define ASSERT
#ifdef ASSERT

void assertion_failure(char *exp, char *file, char *basefile, int line);

#define assert(exp) if(exp);\
else assertion_failure(#exp,__FILE__,__BASE_FILE__,__LINE_);
#else
#define assert(exp)
#endif

#endif