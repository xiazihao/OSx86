//
// Created by xiazihao on 17-2-21.
//

#ifndef OSX86_USR_H
#define OSX86_USR_H

#include "hd.h"

int read_hd(void *buf, int bufsize, int drive, int sector);

int write_hd(void *buf, int bufsize, int drive, int sector);

int wait(int millsec);

int get_ticks();

int open_hd();

int partition_infomation(PartitionInfomation *addr, int device);

int open(char *name, int name_len, int flags);

#endif //OSX86_USR_H
