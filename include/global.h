#ifndef CHP6_GLOBAL_H
#define CHP6_GLOBAL_H

#include "protect.h"
#include "process.h"
#include "console.h"
#include "tty.h"

extern Process *p_proc_ready;
extern int disp_pos;
extern u8 gdt_ptr[6];    /* 0~15:Limit  16~47:Base */
extern Descriptor gdt[GDT_SIZE];
extern u8 idt_ptr[6];    /* 0~15:Limit  16~47:Base */
extern Gate idt[IDT_SIZE];
extern Tss tss;
//========================================
extern Process process_table[];

extern char task_stack[];
extern u32 k_reenter;
extern Task task_table[];
extern Task user_proc_table[];
extern irq_handler irq_table[];
extern system_call sys_call_table[];
extern int ticks;
extern TTY tty_table[NR_CONSOLES];
extern Console console_table[NR_CONSOLES];
extern int nr_current_console;
#endif