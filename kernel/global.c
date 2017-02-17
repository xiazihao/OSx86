#define GLOBAL_VARIABLES_HERE

#include "global.h"
#include "proto.h"

extern void testA();

extern void testB();

extern void task_tty();

extern void IDLE();

extern void task_hd();

extern void systask();


int sys_sendmessage(Process *process, int function, int dest, Message *message);

int sys_receivemessage(Process *process, int function, u32 src, Message *message);

Process process_table[NR_TASKS + NR_PROCS];
char task_stack[STACK_SIZE_TOTAL];
Process *p_proc_ready;
int disp_pos;
u8 gdt_ptr[6];    /* 0~15:Limit  16~47:Base */
Descriptor gdt[GDT_SIZE];
u8 idt_ptr[6];    /* 0~15:Limit  16~47:Base */
Gate idt[IDT_SIZE];
Tss tss;
u32 k_reenter;
Task task_table[NR_TASKS] = {{task_tty, STACK_SIZE_TTY, "keyboard"},
                             {systask,  STACK_SYSTASK,  "system task"},
                             {task_hd,  STACK_HD,       "hd"}};
Task user_proc_table[NR_PROCS] = {{testA, STACK_SIZE_TESTA, "testA"},
                                  {testB, STACK_SIZE_TESTB, "testB"},
                                  {IDLE,  STACK_IDLE,       "idle"}};
irq_handler irq_table[NR_IRQ];
system_call sys_call_table[NR_SYS_CALL] = {sys_sendmessage, sys_receivemessage, sys_write};
int ticks;
TTY tty_table[NR_CONSOLES];
Console console_table[NR_CONSOLES];

int nr_current_console;
