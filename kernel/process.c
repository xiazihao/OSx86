#include <lib.h>
#include <process.h>
#include <systask.h>
#include <const.h>
#include <global.h>
#include <protect.h>
#include <proto.h>
#include <type.h>
#include <process.h>
#include <hd.h>
#include <usr.h>

MessageChain messageQueue[QUEUESIZE];

static u32 get_pid(Process *process);

static int physic_copy(void *dest, void *src, int size);

static u32 get_linear_addr(Process *process, void *virtual_addr);

static MessageChain *getQueuePosition();

/**
 * Get empty queueposition.
 * @return return empty messagequeue address, retur NULL when messagequeue if full
 */
static MessageChain *getQueuePosition() {
    for (int i = 0; i < QUEUESIZE; ++i) {
        if (messageQueue[i].active == FALSE) {
            return &messageQueue[i];
        }
    }
    return NULL;
}

/**
 * Send interrupt signal to process and inform process to handler interrupt.
 * @param handlerPid  the interrupt handler process' pid
 * @return
 */
int inform_interrupt(u32 handlerPid) {
    assert(handlerPid < NR_PROCS + NR_TASKS - 1);
    Process *process = &process_table[handlerPid];
//    if (process->receivefrom == INTERRUPT || process->receivefrom == ANY) {
    process->interrupt = TRUE;
    p_proc_ready = process;
}

/**
 * Process schedule function.
 */
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

/**
 * Init message queue.
 */
void init_queue() {
    for (int i = 0; i < QUEUESIZE; ++i) {
        memset(&messageQueue[i], 0, sizeof(MessageChain));
    }
}

/**
 * System call, send function in IPC
 * @param process sender process
 * @param function process method type, function could be:
 * @param dest destination process' pid
 * @param message message struct's address, note:kernel and process are not in same address space
 * @return 0:send message success, 1:destination refuse to receive, 2:no enough empty message queue...
 */
int sys_sendmessage(Process *process, int function, int dest, Message *message) {
    assert(get_pid(process) != dest);
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
            physic_copy(&receiver->queue.start->message, (void *) get_linear_addr(sender, message), sizeof(Message));
            receiver->queue.start->prev = NULL;
            receiver->queue.start->next = NULL;
            receiver->queue.start->message.sender = sender->pid;
            receiver->queue.start->active = TRUE;
        } else { // insert at start
            temp = getQueuePosition();
            if (temp == NULL) {
                return 2;
            }
            physic_copy(temp, (void *) get_linear_addr(sender, message), sizeof(Message));
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
//        p_proc_ready = receiver;
        return 0;//ok
    }
    return 1;//dest is not allowed receive
}

/**
 * System call, receive message in IPC.
 * @param process receiver process
 * @param function receive method type, function could be: INT, INFORM, RECEIVE ...
 * @param src receiver process allowed PID
 * @param message receiver process message buf's address
 * @return 0:success 1: 2: ...
 */
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
                physic_copy((void *) get_linear_addr(process, message), &temp->message, sizeof(Message));
                temp->active = FALSE;
                process->queue.count--;
                return 0;//get queue
            }
            // no message in queque
            assert(process->queue.count == 0);
            schedule();
            return 1;
        default:
            break;
    }
    return 2;
}

static u32 get_linear_addr(Process *process, void *virtual_addr) {
    Descriptor *descriptor = &(process->ldts[INDEX_LDT_RW]);
    u32 segBase = descriptor->base_low | descriptor->base_mid << 16 | descriptor->base_high << 24;
    return segBase + (u32) virtual_addr;
}

static int physic_copy(void *dest, void *src, int size) {
    u8 *pD = dest;
    u8 *pS = src;
    for (int i = 0; i < size; ++i) {
        *pD = *pS;
        pD++;
        pS++;
    }
}

void *virtual2Linear(u32 pid, void *virtual) {
    assert(pid >= 0 && pid < NR_TASKS + NR_CONSOLES);
    Descriptor *descriptor = &(process_table[pid].ldts[INDEX_LDT_RW]);
    return (void *) (descriptor->base_low | descriptor->base_mid << 16 | descriptor->base_high << 24) + (u32) virtual;

}

static u32 get_pid(Process *process) {
    return process->pid;
}

typedef struct {
    u8 status;//0x80 active part, 0x00 inactive part
    u8 startHead;
    u8 startSector;//0 - 5 bits
    u8 startCylinder;//6 - 7 and all
    u8 sysId;
    u8 endHead;
    u8 endSector;//0 - 5
    u8 endCylinder;//6 - 7 and all
    u32 startSectorFromStart;
    u32 sectorCount;
} Entry;

void testA() {
//    PartitionInfomation partitionInfomation;
//    u8 buf[512];
//    open_hd();
//    partition_infomation(&partitionInfomation, 6);
//    Entry *entry = &buf[0x1BE];
//    read_hd(buf, 512, 0, 0);
//    buf[100] = 'h';
//    buf[101] = 'e';
//    buf[102] = 0;
//    write_hd(buf, 512, 0, 2);
//    wait(1000);
//    read_hd(buf, 512, 0, 2);
    while (1) {
//        printf("\nreceive sector\n");
//        printf("start: %x \n size: %x", partitionInfomation.start, partitionInfomation.size);
//        printf("sys id: %x\n", entry->sysId);
//        entry++;
//        printf("sys id: %x\n", entry->sysId);
//        printf("buf:%s", &buf[100]);
        while (1);
        wait(10000);
//        printf("A:%d  ", get_ticks());
    }
}

void testB() {
    while (1) {
//        printf("B");
//        get_ticks();
        wait(5000);
    }
}

void IDLE() {
    while (1) {
    }
}