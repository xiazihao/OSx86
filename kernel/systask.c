#include <process.h>
#include <proto.h>
#include <global.h>
#include <lib.h>

//
// Created by xiazihao on 2/11/17.
//
#define SYSTASKPID 1

void systask() {
    MESSAGE message;
    memset(&message,0,sizeof(MESSAGE));
    u32 side;
    while (1) {
        receivemessage(0, ANY, &message);
        side = message.obj;
        assert(message.type == 1);
//        printf("side:%d", side);
//        assert(side == 2);
//        while (1);
        message.msg1.m1i1 = ticks;
        sendmessage(0, 2, &message);

    }

}

int get_ticks() {
    MESSAGE message;
    message.type = 1;
    message.msg1.m1i1 = 1;
    if (!sendmessage(0, SYSTASKPID, &message)) {
        receivemessage(0, ANY, &message);
    }
    return message.msg1.m1i1;


}

void IDLE() {
//    printf("idle is running");
    while (1) {
    }
}
