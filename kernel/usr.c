#include <hd.h>
#include <proto.h>
#include <systask.h>

//
// Created by xiazihao on 17-2-21.
//
int read_hd(void *buf, int bufsize, int drive, int sector) {
    Message message;
    message.type = DEV_READ;
// m2i1:bufsize     m2i2:secotr     m2i3:drive   m2p4:address
    message.msg2.m2i1 = bufsize;
    message.msg2.m2i2 = sector;
    message.msg2.m2i3 = drive;
    message.msg2.m2p4 = buf;
    sendmessage(0, PID_HD, &message);
    while (receivemessage(RECEIVE, ANY, &message));
}

int write_hd(void *buf, int bufsize, int drive, int sector) {
    Message message;
    message.type = DEV_WRITE;
    // m2i1:bufsize     m2i2:secotr     m2i3:drive   m2p4:address
    message.msg2.m2i1 = bufsize;
    message.msg2.m2i2 = sector;
    message.msg2.m2i3 = drive;
    message.msg2.m2p4 = buf;
    sendmessage(0, PID_HD, &message);
    while (receivemessage(RECEIVE, ANY, &message));
}