#define GLOBAL_VARIABLES_HERE

#include "global.h"
#include "proto.h"
extern void testA();

extern void testB();

extern void task_tty();

PROCESS process_table[NR_TASKS + NR_PROCS];
char task_stack[STACK_SIZE_TOTAL];
PROCESS *p_proc_ready;
int disp_pos;
u8 gdt_ptr[6];    /* 0~15:Limit  16~47:Base */
DESCRIPTOR gdt[GDT_SIZE];
u8 idt_ptr[6];    /* 0~15:Limit  16~47:Base */
GATE idt[IDT_SIZE];
TSS tss;
u32 k_reenter;
TASK task_table[NR_TASKS] = {{task_tty, STACK_SIZE_TTY, "keyboard"}};
TASK user_proc_table[NR_PROCS] = {{testA, STACK_SIZE_TESTA, "testA"},
                                  {testB, STACK_SIZE_TESTB, "testB"}};
irq_handler irq_table[NR_IRQ];
public system_call sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_write};
int ticks;
TTY tty_table[NR_CONSOLES];
CONSOLE console_table[NR_CONSOLES];

int nr_current_console;
