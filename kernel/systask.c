#include <process.h>
#include <proto.h>
#include <global.h>
#include <lib.h>
#include <systask.h>

//
// Created by xiazihao on 2/11/17.
//
#define SYSTASKPID 1


void systask() {
    MESSAGE message;
    memset(&message, 0, sizeof(MESSAGE));
    u32 side;
    while (1) {
        while (receivemessage(0, ANY, &message));//return 0: receive
        side = message.sender;
        switch (message.type) {
            case SYSGETTICKS:
                message.msg1.m1i1 = ticks;
                sendmessage(0, side, &message);
                break;
        }

//        printf("side: %d", side);
//        assert(side == 2);

    }

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
