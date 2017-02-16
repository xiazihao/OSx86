#include <process.h>
#include <proto.h>
#include <global.h>
#include <lib.h>
#include <systask.h>

//
// Created by xiazihao on 2/11/17.
//
#define SYSTASKPID 1

struct s_waitQueue {
    int active;
    int ticks;
    int sender;
};

void systask() {
    MESSAGE message;
    struct s_waitQueue waitQueue;
    memset(&message, 0, sizeof(MESSAGE));
    u32 side;
    while (1) {
//        while (receivemessage(0, ANY, &message));//return 0: receive
        while (TRUE) { // systask working loop
            if (!receivemessage(0, ANY, &message)) {
                break;
            }

            if (waitQueue.active == TRUE) {
//                printf("check wait");
                if (waitQueue.ticks < ticks) {
                    sendmessage(0, waitQueue.sender, &message);
                    printf("send wait");
                    waitQueue.active = FALSE;
                }
            }

        }
        side = message.sender;
        switch (message.type) {
            case SYSGETTICKS:
                message.msg1.m1i1 = ticks;
                sendmessage(0, side, &message);
                break;
            case SYSWAIT:
                waitQueue.sender = side;
//                assert(message.msg1.m1i1 == 100);
                waitQueue.ticks = ticks + (message.msg1.m1i1 / (1000 / HZ));
                waitQueue.active = TRUE;
                printf("get wait");
                break;
            default:
                break;
        }

    }

}

int wait(int millsec) {
    MESSAGE message;
    memset(&message, 0, sizeof(MESSAGE));
    message.type = SYSWAIT;
    message.msg1.m1i1 = millsec;
    if (!sendmessage(0, SYSTASKPID, &message)) {
        while (receivemessage(0, ANY, &message));
        return 0;
    }
    return 1;
}

int get_ticks() {
    MESSAGE message;
    memset(&message, 0, sizeof(MESSAGE));
    message.type = SYSGETTICKS;
    if (!sendmessage(0, SYSTASKPID, &message)) {
        while (receivemessage(0, ANY, &message));
        return message.msg1.m1i1;
    }
    return 0;
}

void IDLE() {
//    printf("idle is running");
    while (1) {
    }
}
