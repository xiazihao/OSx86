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
void out_byte(u16 port, u8 value);

u8 in_byte(u16 port);

void disp_str(char *info);

void disp_color_str(char *info, int color);

void disp_int(int input);

//i8259.h
void init_8259A();

void putIrqHandler(int irq, irq_handler handler);

//kernel.asm
void restart();

void disable_irq(int irq);

void enable_irq(int irq);

void disable_int();

void enable_int();

void port_read(u16 port, void *buf, int n);

void port_write(u16 port, void *buf, int n);

//clock.c
void milli_delay(int milli_sec);

//vsprintf.c
int vsprintf(char *buf, const char *format, va_list args);

//printf.c
int printf(const char *format, ...);

//tty.c
int sys_write(Process *p_process, char *buf, int len);

//syscall.asm
void write(char *buf, int len);

int sendmessage(int function, u32 dest, Message *message);

int receivemessage(int function, u32 src, Message *message);

#endif
