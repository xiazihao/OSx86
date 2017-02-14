#include <lib.h>
#include <process.h>
#include "const.h"
#include "global.h"
#include "protect.h"
#include "proto.h"
#include "type.h"
#include "process.h"

static u32 getPid(PROCESS *process);

static int physicCopy(void *dest, void *src, int size);

static u32 getLinearAddr(PROCESS *process, void *virtualAddr);

public int sys_get_ticks() {
    return ticks;
}

public void schedule() {
    if (p_proc_ready->ticks > 0) {
        return;
    }
    p_proc_ready->ticks = p_proc_ready->priority;
    do {
        p_proc_ready++;
        if (p_proc_ready >= process_table + NR_TASKS + NR_PROCS) {
            p_proc_ready = process_table;
            assert(p_proc_ready[NR_TASKS + NR_PROCS - 1].status == RUNNABLE);
        }
        if (p_proc_ready->status == RUNNABLE) {
            break;
        }
    } while (TRUE);
}


int sys_sendmessage(PROCESS *process, int function, u32 dest, MESSAGE *message) {
    assert(getPid(process) != dest);
    assert(dest < NR_PROCS + NR_TASKS);
    PROCESS *sender = process;
    PROCESS *receiver = &process_table[dest];
    if (receiver->receivefrom == ANY || receiver->receivefrom == dest) {
        assert(receiver->message != NULL);
        u32 srcLinear = getLinearAddr(sender, message);
        u32 destLinear = getLinearAddr(receiver, receiver->message);
        physicCopy((void*)destLinear, (void*)srcLinear, sizeof(MESSAGE));
        receiver->status = RUNNABLE;
        return 0;
    }
    return 1;

}

int sys_receivemessage(PROCESS *process, int function, u32 src, MESSAGE *message) {
    assert(message != NULL);
    assert(src == ANY || src == NOTASK || src < (NR_TASKS + NR_PROCS));
    process->receivefrom = src;
    process->message = message;
    process->status = RECEVING;
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

static u32 getPid(PROCESS *process) {
    return process->pid;
}

void testA() {
    MESSAGE message;
    message.msg1.m1i1 = 10;
    message.type = 1;
    int result;
    while (1) {
//        result = sendmessage(0, NR_TASKS + 1, &message);
//        printf("result: %d\n", result);
//        printf("ticks: %d", get_ticks());
//        assert(1 > 2);
        milli_delay(10000);
        milli_delay(10000);
        milli_delay(10000);
        milli_delay(10000);
    }
}

void testB() {
    MESSAGE message;
    while (1) {
//        receivemessage(0, ANY, &message);
//        printf("B: %d %d\n", message.msg1.m1i1,message.type);
        milli_delay(10000);
    }
}