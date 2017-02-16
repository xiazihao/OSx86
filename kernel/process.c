#include <lib.h>
#include <process.h>
#include <systask.h>
#include "const.h"
#include "global.h"
#include "protect.h"
#include "proto.h"
#include "type.h"
#include "process.h"

MESSAGECHAIN messageQueue[QUEUESIZE];

static u32 getPid(PROCESS *process);

static int physicSet(void *dst, u8 value, int size);

static int physicCopy(void *dest, void *src, int size);

static u32 getLinearAddr(PROCESS *process, void *virtualAddr);

static MESSAGECHAIN *getQueuePosition();

static MESSAGECHAIN *getQueuePosition() {
    for (int i = 0; i < QUEUESIZE; ++i) {
        if (!messageQueue[i].active) {
            return &messageQueue[i];
        }
    }
    return NULL;
}

//public int sys_get_ticks() {
//    return ticks;
//}

void schedule() {
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

void init_queue() {
    for (int i = 0; i < QUEUESIZE; ++i) {
        memset(&messageQueue[i], 0, sizeof(MESSAGECHAIN));
//        messageQueue[i].next = NULL;
//        messageQueue[i].prev = NULL;
    }
}

int sys_sendmessage(PROCESS *process, int function, int dest, MESSAGE *message) {
    assert(getPid(process) != dest);
    assert(dest < NR_PROCS + NR_TASKS);
    MESSAGECHAIN *temp;
    PROCESS *sender = process;
    PROCESS *receiver = &process_table[dest];
    if (receiver->receivefrom == ANY || receiver->receivefrom == dest) {
        if (receiver->queue.count == 0) { //it is receiver's first message, create a queue
            assert(receiver->queue.last == NULL);
            assert(receiver->queue.start == NULL);
            receiver->queue.start = getQueuePosition();
            if (receiver->queue.start == NULL) {
                return 2;//get queque faild
            }
            receiver->queue.last = receiver->queue.start;
            receiver->queue.count++;
            assert(receiver->queue.count == 1);
            physicCopy(&receiver->queue.start->message, (void *) getLinearAddr(sender, message), sizeof(MESSAGE));
            receiver->queue.start->prev = NULL;
            receiver->queue.start->next = NULL;
            receiver->queue.start->message.sender = sender->pid;
            receiver->queue.start->active = TRUE;
        } else { // insert at start
            temp = getQueuePosition();
            if (temp == NULL) {
                return 2;
            }
            physicCopy(temp, (void *) getLinearAddr(sender, message), sizeof(MESSAGE));
            temp->next = receiver->queue.start;
            temp->prev = NULL;
            temp->active = TRUE;
            receiver->queue.start->prev = temp;
            receiver->queue.start = temp;
            if (receiver->queue.count > 1) {
                assert(receiver->queue.start->next->prev == receiver->queue.start);
                assert(receiver->queue.last->prev->next == receiver->queue.last);
            }

            receiver->queue.count++;
        }
        receiver->status = RUNNABLE;
        if (receiver->queue.count == 1) {
            assert(receiver->queue.last == receiver->queue.start);
        }
        assert(receiver->queue.start != NULL);
        assert(receiver->queue.last != NULL);
        assert(receiver->queue.count != NULL);
        return 0;//ok
    }
    return 1;//dest is not allowed receive
}

int sys_receivemessage(PROCESS *process, int function, u32 src, MESSAGE *message) {
    assert(message != NULL);
    assert(src == ANY || src == NOTASK || src < (NR_TASKS + NR_PROCS));
    assert(process->status == RUNNABLE);
    process->receivefrom = src;
    MESSAGECHAIN *temp;
    if (process->queue.count > 0) {
        assert(process->queue.last != NULL);
        temp = process->queue.last;
        if (process->queue.count == 1) {//only one message
            assert(process->queue.start == process->queue.last);
            process->queue.start = NULL;
            process->queue.last = NULL;
        } else {
            assert(temp->prev != NULL);
            assert(temp->prev->next == temp);
            temp->prev->next = NULL;
            process->queue.last = temp->prev;
            if (process->queue.count == 1) {
                assert(process->queue.start == process->queue.last);
            }
        }
        physicCopy((void *) getLinearAddr(process, message), &temp->message, sizeof(MESSAGE));
        temp->active = FALSE;
        process->queue.count--;
        return 0;//get queue
    }
    // no message in quequ
//    process->status = RECEVING;
    assert(process->queue.count == 0);
    schedule();
    return 1;
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
    int result;
    int count = 0;
    while (1) {
        printf("A:%d  ", get_ticks());
//        milli_delay(1000);
        wait(100);

    }
}

void testB() {
    while (1) {
//        printf("B: %d", get_ticks());
        milli_delay(1000);
    }
}