#ifndef __const_H
#define __const_H
#define public
#define private static
#define GDT_SIZE 128
#define IDT_SIZE 256
#define NR_IRQ 16
//8259 interrupt
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21 //ocw2
#define INT_S_CTL 0xa0
#define INT_S_CTLMASK 0xa1    //ocw2
#define EXTERN extern
//PIT clock
#define TIMER0 0x40
#define TIME_MODE 0x43
#define RATE_GENERATOR 0x34
#define TIMER_FREQ  1193182L
#define HZ  100
//request privilege
#define    RPL_KRNL    SA_RPL0
#define    RPL_TASK    SA_RPL1
#define    RPL_USER    SA_RPL3
//privilege
#define    PRIVILEGE_KRNL    0
#define    PRIVILEGE_TASK    1
#define    PRIVILEGE_USER    3
//VGA
#define CTRC_ADDR_REG 0x3D4
#define CTRC_DATA_REG 0x3D5
#define START_ADDR_H    0xc
#define START_ADDR_L    0xD
#define CURSOR_H    0xE
#define CURSOR_L    0xF
#define V_MEM_BASE  0xb8000
#define V_MEM_SIZE  0x8000

//interrupt index
#define CLOCK_IRQ 0
#define KEYBOARD_IRQ 1
#define CASCADE_IRQ	2
#define AT_WINI_IRQ 14

//tty
#define NR_CONSOLES 3
//system call
#define NR_SYS_CALL 3
#endif