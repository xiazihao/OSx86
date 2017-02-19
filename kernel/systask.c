#include <process.h>
#include <proto.h>
#include <global.h>
#include <lib.h>
#include <systask.h>

//
// Created by xiazihao on 2/11/17.
//
static int up();


typedef struct s_waitQueue {
    int ticks;
    int sender;
} WaitQueue;
#define SIZE 16
int last;
WaitQueue queue[SIZE];

static int insert(int ticks, unsigned int sender) {
    if (last + 1 < SIZE) {
        last++;
        queue[last].ticks = ticks;
        queue[last].sender = sender;
        up();
        return 0;
    }
    return 1;//1:not enough space
}

static int up() {
    int position = last;
    int parrent = position / 2;
    WaitQueue swich;
    while (position > 1) {
        if (queue[position].ticks < queue[parrent].ticks) {
            swich.ticks = queue[position].ticks;
            swich.sender = queue[position].sender;
            queue[position].ticks = queue[parrent].ticks;
            queue[position].sender = queue[parrent].sender;
            queue[parrent].ticks = swich.ticks;
            queue[parrent].sender = swich.sender;
        }
        position = parrent;
        parrent = parrent / 2;
    }
}

static WaitQueue pop() {

}

static int down() {
    if (last < 1) {
        return 0;
    }
    int position = 1;
    int son = position * 2;
    while (son <= last) {
        if (son == last) {
            memcpy(&queue[position], &queue[son], sizeof(WaitQueue));
//            queue[position].ticks = queue[son].ticks;
//            queue[position].sender = queue[son].sender;
            position = son;
            son = position * 2;
        } else if (queue[son].ticks < queue[son + 1].ticks) {
            memcpy(&queue[position], &queue[son], sizeof(WaitQueue));
//            queue[position].ticks = queue[son].ticks;
//            queue[position].sender = queue[son].sender;
            position = son;
            son = position * 2;
        } else {
            memcpy(&queue[position], &queue[son + 1], sizeof(WaitQueue));
//            queue[position].ticks = queue[son + 1].ticks;
//            queue[position].sender = queue[son + 1].sender;
            position = son + 1;
            son = position * 2;
        }
    }
    last--;
    return 1;
}

typedef struct {
    u32 baseLow;
    u32 baseHigh;
    u32 lengthLow;
    u32 lengthHigh;
    u32 type;
} ARDS;

int getPhysicalSize() {
    unsigned int total = 0;
    unsigned int *num =(unsigned int *) 0x91000;
    ARDS *ards = (ARDS*)0x91010;
    printf("number: %d \n", *num);
    for (int i = 0; i < *num; ++i) {
        printf("baselow: %x,basehight: %x, lengthlow: %x, lengthhigh: %x, type: %d\n", ards->baseLow, ards->baseHigh,
               ards->lengthLow, ards->lengthHigh, ards->type);
        if (ards->type == 1) {
            total += ards->lengthLow;
        }
        ards++;
    }
    printf("total: %x", total);

}

void systask() {

    Message message;
    memset(&message, 0, sizeof(Message));
    u32 side;
    last = 0;
    getPhysicalSize();
    while (1) {

        memset(&message, 0, sizeof(Message));
        while (TRUE) { // systask working loop
            if (!receivemessage(0, ANY, &message)) {
                break;
            }
            if (last > 0) {
                if (ticks > queue[1].ticks) {
                    message.msg1.m1i1 = 0;
                    sendmessage(0, queue[1].sender, &message);
                    down();
                }
            }

        }
        side = message.sender;
        switch (message.type) {
            case SYSGETTICKS:
                message.msg1.m1i1 = ticks;
//                printf("tick: %d", side);
                sendmessage(0, side, &message);
                break;
            case SYSWAIT:
                if (insert(ticks + (message.msg1.m1i1 / (1000 / HZ)), side)) { // return not 1 enter
                    message.msg1.m1i1 = 1;
                    sendmessage(0, side, &message);
                }
//                printf("sys: %d", side);
                break;
            default:
                break;
        }


    }

}

int wait(int millsec) {
    Message message;
    receivemessage(INFORM, ANY, NULL);
    memset(&message, 0, sizeof(Message));
    message.type = SYSWAIT;
    message.msg1.m1i1 = millsec;
    if (!sendmessage(RECEIVE, PID_SYSTASK, &message)) {
        while (receivemessage(0, PID_SYSTASK, &message));
        if (message.msg1.m1i1) {
            return 1;
        }
        return 0;
    }
    return 1; // send faild
}

/**
 * System call function, get ticks from start of os.
 * @return return 0: get ticks failed
 */
int getTicks() {
    Message message;
    receivemessage(INFORM, ANY, NULL);
    memset(&message, 0, sizeof(Message));
    message.type = SYSGETTICKS;
    if (!sendmessage(RECEIVE, PID_SYSTASK, &message)) {
        while (receivemessage(RECEIVE, PID_SYSTASK, &message));
        return message.msg1.m1i1;
    }
    return 0;
}


