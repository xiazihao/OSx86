#ifndef __type_h
#define __type_h
typedef unsigned int u32;

typedef unsigned short u16;

typedef unsigned char u8;

typedef void (*int_handler)();

typedef void (*task_f)();

typedef void (*irq_handler)(int irq);

typedef void *system_call;

typedef char *va_list;

typedef char bool;

#define NULL    0
#define TRUE    1
#define FALSE   0
#endif