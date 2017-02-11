#ifndef CHP6_GLOBAL_H
#define CHP6_GLOBAL_H

#include "protect.h"
#include "process.h"
#include "console.h"
#include "tty.h"

extern PROCESS *p_proc_ready;
extern int disp_pos;
extern u8 gdt_ptr[6];    /* 0~15:Limit  16~47:Base */
extern DESCRIPTOR gdt[GDT_SIZE];
extern u8 idt_ptr[6];    /* 0~15:Limit  16~47:Base */
extern GATE idt[IDT_SIZE];
extern TSS tss;
//========================================
extern PROCESS process_table[];

extern char task_stack[];
extern u32 k_reenter;
extern TASK task_table[];
extern TASK user_proc_table[];
extern irq_handler irq_table[];
extern system_call sys_call_table[];
extern int ticks;
extern TTY tty_table[NR_CONSOLES];
extern CONSOLE console_table[NR_CONSOLES];
extern int nr_current_console;
#endif