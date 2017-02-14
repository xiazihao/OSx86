#ifndef __process_h
#define __process_h

#include "const.h"
#include "protect.h"

typedef struct s_stackframe {
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;//puashad
    u32 esi;
    u32 ebp;
    u32 kernel_esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;//pushad
    u32 retaddr;//push return address after call "save"
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} STACKFRAME;

struct message1 {
    int m1i1;
    int m1i2;
    int m1i3;
    int m1i4;
};
typedef struct {
    int type;
    int dest;
    union {
        struct message1 msg1;
    };
} MESSAGE;
typedef struct s_proc {
    STACKFRAME regs; // must be the first memeber of struct
    u16 ldt_sel;//ldt selector
    DESCRIPTOR ldts[LDT_SIZE];//0:code seg 1:data seg
    //above is essential and the sequence cannot be changed
    int ticks;
    int priority;
    u32 pid;
    char name[16];
    int nrtty;//index of tty
    int status;//receving sending runable
    MESSAGE *message;
    u32 receivefrom;

} PROCESS;


typedef struct s_task {
    task_f initial_eip;
    int stacksize;
    char name[32];
} TASK; // the essential info to create a process
//recvivefrom
#define ANY 0xfffffff0
#define NOTASK (ANY - 0x1)

//status
#define RUNNABLE    0
#define RECEVING    1
#define SENDING     1<<1


#define NR_PROCS    3
#define NR_TASKS    2


public int sys_get_ticks();

public void sys_call();

public int get_ticks();

public void schedule();

#define STACK_SIZE_TESTA    0x8000
#define STACK_SIZE_TESTB    0x8000
#define STACK_SIZE_TTY      0x8000
#define STACK_SYSTASK       0x8000
#define STACK_IDLE          0x8000
#define STACK_SIZE_TOTAL    STACK_SIZE_TESTA + STACK_SIZE_TESTB + STACK_SIZE_TTY + STACK_SYSTASK + STACK_IDLE
#endif