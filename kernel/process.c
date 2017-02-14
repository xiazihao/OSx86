#include <lib.h>
#include <process.h>
#include "const.h"
#include "global.h"
#include "protect.h"
#include "proto.h"
#include "type.h"
#include "process.h"

static u32 getPid(PROCESS *process);

static int physicSet(void *dst, u8 value, int size);

static int physicCopy(void *dest, void *src, int size);

static u32 getLinearAddr(PROCESS *process, void *virtualAddr);

public int sys_get_ticks() {
    return ticks;
}

public void schedule() {
    if (p_proc_ready->ticks > 0 && p_proc_ready->status == RUNNABLE) {
        return;
    }
    if (p_proc_ready->ticks <= 0) {
        p_proc_ready->ticks = p_proc_ready->priority;
    }
    do {
        p_proc_ready++;
        if (p_proc_ready >= process_table + NR_TASKS + NR_PROCS) {
            p_proc_ready = process_table;
            assert(p_proc_ready[NR_TASKS + NR_PROCS - 1].status == RUNNABLE);//IDLE task should be always RUNNABLE
        }
        if (p_proc_ready->status == RUNNABLE) {
            break;
        }
    } while (TRUE);
}


int sys_sendmessage(PROCESS *process, int function, int dest, MESSAGE *message) {
    assert(getPid(process) != dest);
    assert(dest < NR_PROCS + NR_TASKS);
    PROCESS *sender = process;
    PROCESS *receiver = &process_table[dest];
    assert(dest == receiver->pid);
    if (process->status != RUNNABLE) {
        return 2;
    }
    if (receiver->receivefrom == ANY || receiver->receivefrom == dest) {
        assert(receiver->message != NULL);
        MESSAGE temp;
        physicSet(&temp, 0, sizeof(MESSAGE));
        u32 srcLinear = getLinearAddr(sender, message);
        u32 destLinear = getLinearAddr(receiver, receiver->message);
        physicCopy(&temp, (void *) srcLinear, sizeof(MESSAGE));
        temp.obj = sender->pid;
        physicCopy((void *) destLinear, &temp, sizeof(MESSAGE));
        ((MESSAGE*) destLinear)->obj = sender->pid;
//        physicCopy((void *) destLinear, (void *) srcLinear, sizeof(MESSAGE));
        receiver->status = RUNNABLE;
        assert(receiver->status == RUNNABLE);
//        schedule();
        return 0;
    }
    return 1;
}

int sys_receivemessage(PROCESS *process, int function, u32 src, MESSAGE *message) {
    assert(message != NULL);
    assert(src == ANY || src == NOTASK || src < (NR_TASKS + NR_PROCS));
    assert(process->status == RUNNABLE);
    process->receivefrom = src;
    process->message = message;
    process->status = RECEVING;
    assert(process->status == RECEVING);
    schedule();
}

static u32 getLinearAddr(PROCESS *process, void *virtualAddr) {
    DESCRIPTOR *descriptor = &(process->ldts[INDEX_LDT_RW]);
    u32 segBase = descriptor->base_low | descriptor->base_mid << 16 | descriptor->base_high << 24;
    return segBase + (u32) virtualAddr;
}

static int physicCopy(void *dest, void *src, int size) {
    u8 *pD = dest;
    u8 *pS = src;
    for (int i = 0; i < size; ++i) {
        *pD = *pS;
        pD++;
        pS++;
    }
}

static int physicSet(void *dst, u8 value, int size) {
    char *p_d = (char *) dst;
    for (int i = 0; i < size; ++i) {
        *p_d++ = value;
    }
}

static u32 getPid(PROCESS *process) {
    return process->pid;
}

void testA() {
    MESSAGE message;
    memset(&message, 0, sizeof(MESSAGE));
    message.msg1.m1i1 = 10;
    while (1) {
        printf("ticks: %d", get_ticks());
        milli_delay(10000);
    }
}

void testB() {
    while (1) {
        milli_delay(10000);
    }
}