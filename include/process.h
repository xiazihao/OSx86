#ifndef __process_h
#define __process_h

#include "const.h"
#include "protect.h"
#include "fs.h"

/**
 * Max number of files per process could open
 */
#define NR_FILES   10

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
} StackFrame;


struct msg1 {
    int m1i1;
    int m1i2;
    int m1i3;
    int m1i4;
};
struct msg2 {
    int m2i1;
    int m2i2;
    int m2i3;
    void *m2p4;
};

typedef struct s_message {
    int type;
    u32 sender;
    union {
        struct msg1 msg1;
        struct msg2 msg2;
    };
} Message;

typedef struct s_messageQueueElement MessageChain;

typedef struct s_messageQueueElement {
    Message message;
    MessageChain *next;
    MessageChain *prev;
    int active;
} MessageChain;


typedef struct {
    MessageChain *start;
    MessageChain *last;
    int count;
} MessageQueue;


typedef struct s_proc {
    StackFrame regs; // must be the first memeber of struct
    u16 ldt_sel;//ldt selector
    Descriptor ldts[LDT_SIZE];//0:code seg 1:data seg
    //above is essential and the sequence cannot be changed
    int ticks;
    int priority;
    u32 pid;
    char name[16];
    int nrtty;//index of tty
    int status;//receving sending runable
    int interrupt;
    u32 receivefrom;
    MessageQueue queue;
    FileDescriptor *fileDescriptor[NR_FILES];
} Process;


typedef struct s_task {
    task_f initial_eip;
    int stacksize;
    char name[32];
} Task; // the essential info to create a process
//recvivefrom
#define ANY 0xfffffff0
#define NOTASK (ANY - 0x01)
#define INTERRUPT (ANY - 0x02)
//status
#define RUNNABLE    0
#define RECEVING    1
#define SENDING     1<<1

//function
#define RECEIVE    0x00
#define INFORM  0x01
#define INT     0x02


#define NR_PROCS    3
#define NR_TASKS    4


#define QUEUESIZE  128

//int sys_get_ticks();

void sys_call();

int inform_interrupt(u32 handlerPid);

void schedule();

void init_queue();

void *virtual2Linear(u32 pid, void *virtual);

#define STACK_SIZE_TESTA    0x8000
#define STACK_SIZE_TESTB    0x8000
#define STACK_SIZE_TTY      0x8000
#define STACK_SYSTASK       0x8000
#define STACK_HD            0x8000
#define STACK_FS            0x8000
#define STACK_IDLE          0x8000
#define STACK_SIZE_TOTAL    STACK_SIZE_TESTA + STACK_SIZE_TESTB + STACK_SIZE_TTY + STACK_SYSTASK + STACK_IDLE + STACK_HD + STACK_FS
#endif