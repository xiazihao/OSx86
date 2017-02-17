#include <lib.h>
#include <process.h>
#include <systask.h>
#include "const.h"
#include "global.h"
#include "protect.h"
#include "proto.h"
#include "type.h"
#include "process.h"
#include "hd.h"

MessageChain messageQueue[QUEUESIZE];

static u32 getPid(Process *process);

static int physicCopy(void *dest, void *src, int size);

static u32 getLinearAddr(Process *process, void *virtualAddr);

static MessageChain *getQueuePosition();

static MessageChain *getQueuePosition() {
    for (int i = 0; i < QUEUESIZE; ++i) {
        if (messageQueue[i].active == FALSE) {
            return &messageQueue[i];
        }
    }
    return NULL;
}

int informInterrupt(u32 handlerPid) {
    assert(handlerPid < NR_PROCS + NR_TASKS - 1);
    Process *process = &process_table[handlerPid];
//    if (process->receivefrom == INTERRUPT || process->receivefrom == ANY) {
        process->interrupt = TRUE;
        p_proc_ready = process;
//    }
}
//public int sys_get_ticks() {
//    return ticks;
//}

void schedule() {
    if (p_proc_ready->ticks > 0) {
        return;
    }
    if (p_proc_ready->ticks <= 0) {
        p_proc_ready->ticks = p_proc_ready->priority;
    }
    p_proc_ready++;
    if (p_proc_ready >= process_table + NR_TASKS + NR_PROCS) {
        p_proc_ready = process_table;
        assert(p_proc_ready[NR_TASKS + NR_PROCS - 1].status == RUNNABLE);//IDLE task should be always RUNNABLE
    }
}

void initQueue() {
    for (int i = 0; i < QUEUESIZE; ++i) {
        memset(&messageQueue[i], 0, sizeof(MessageChain));
    }
}

int sys_sendmessage(Process *process, int function, int dest, Message *message) {
    assert(getPid(process) != dest);
    assert(dest < NR_PROCS + NR_TASKS);
    MessageChain *temp;
    Process *sender = process;
    Process *receiver = &process_table[dest];
    if ((receiver->receivefrom == ANY) || (receiver->receivefrom == sender->pid)) {
        if (receiver->queue.count == 0) { //it is receiver's first message, create a queue
            assert(receiver->queue.last == NULL);
            assert(receiver->queue.start == NULL);
            receiver->queue.start = getQueuePosition();
            if (receiver->queue.start == NULL) {
                return 2;//get queque faild
            }
            assert(receiver->queue.start != NULL);
            receiver->queue.last = receiver->queue.start;
            receiver->queue.count++;
            assert(receiver->queue.count == 1);
            physicCopy(&receiver->queue.start->message, (void *) getLinearAddr(sender, message), sizeof(Message));
            receiver->queue.start->prev = NULL;
            receiver->queue.start->next = NULL;
            receiver->queue.start->message.sender = sender->pid;
            receiver->queue.start->active = TRUE;
        } else { // insert at start
            temp = getQueuePosition();
            if (temp == NULL) {
                return 2;
            }
            physicCopy(temp, (void *) getLinearAddr(sender, message), sizeof(Message));
            temp->next = receiver->queue.start;
            temp->prev = NULL;
            temp->message.sender = sender->pid;
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
//        schedule();
        return 0;//ok
    }
    return 1;//dest is not allowed receive
}

int sys_receivemessage(Process *process, int function, u32 src, Message *message) {

    assert(src == INTERRUPT || src == ANY || src == NOTASK || src < (NR_TASKS + NR_PROCS));
    assert(process->status == RUNNABLE);
    switch (function) {
        case INT:
            if (process->interrupt) {
                process->interrupt = FALSE;
                return 0;
            } else {
                schedule();
                return 1;
            }
        case INFORM:
            process->receivefrom = src;
            return 0;
        case RECEIVE:
            assert(message != NULL);
            process->receivefrom = src;
            MessageChain *temp;
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
                physicCopy((void *) getLinearAddr(process, message), &temp->message, sizeof(Message));
                temp->active = FALSE;
                process->queue.count--;
                return 0;//get queue
            }
            // no message in quequ
            assert(process->queue.count == 0);
            schedule();
            return 1;
        default:
            break;
    }
    return 2;
}

static u32 getLinearAddr(Process *process, void *virtualAddr) {
    Descriptor *descriptor = &(process->ldts[INDEX_LDT_RW]);
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


static u32 getPid(Process *process) {
    return process->pid;
}

void testA() {

    Message msg;
    msg.type = DEV_OPEN;
    while (1) {
        wait(1000);
        sendmessage(0, HDPID, &msg);
//        printf("A:%d  ", getTicks());
    }
}

void testB() {
    while (1) {
//        printf("B");
//        getTicks();
        wait(5000);
    }
}