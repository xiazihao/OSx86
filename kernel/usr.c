#include <hd.h>
#include <proto.h>
#include <systask.h>
#include <lib.h>
#include <usr.h>

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
//    int t = get_ticks();
//    while ((get_ticks() - t) * 1000 / HZ < 10000) {
//        if (!receivemessage(RECEIVE, ANY, &message)) {
//            printf("read success");
//            return 0;
//        }
//    }
//    return 1;
    while (receivemessage(RECEIVE, ANY, &message));
//
//    int t = get_ticks();
//    while ((get_ticks() - t) * 1000 / HZ < 1000) {
//        if (!receivemessage(0, PID_HD, &message)) {
//            return 0;
//        }
//    }
//    return 1;

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
int get_ticks() {
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

int open_hd() {
    Message message;
    message.type = DEV_OPEN;
    sendmessage(0, PID_HD, &message);
    int t = get_ticks();
    while ((get_ticks() - t) * 1000 / HZ < 1000) {
        if (!receivemessage(0, PID_HD, &message)) {
            return 0;
        }
    }
    return 1;
}

int partition_infomation(PartitionInfomation *addr, int device) {
    //m2i1:type      m2i2:device(obj)    m2i3: m2p4;
    Message message;
    message.type = DEV_IOCTRL;
    message.msg2.m2i1 = IO_PARTITIONINFO;
    message.msg2.m2i2 = 6;
    message.msg2.m2p4 = addr;
    sendmessage(0, PID_HD, &message);
    int t = get_ticks();
    while ((get_ticks() - t) * 1000 / HZ < 1000) {
        if (!receivemessage(0, PID_HD, &message)) {
            return 0;
        }
    }
    return 1;
}

int open(char *name, int name_len, int flags) {
    //m2i1:flags   m2i2:name_len   m2i3        m2p4:name_string
    //int do_open(int flags, int name_len, void *name_string, u32 caller)
    Message message;
    message.type = FILE_OPEN;
    message.msg2.m2i1 = flags;
    message.msg2.m2i2 = name_len;
    message.msg2.m2p4 = name;
    sendmessage(0, PID_FS, &message);
}