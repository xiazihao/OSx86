//
// Created by xiazihao on 2/15/17.
//

#ifndef CHP6_HD_H
#define CHP6_HD_H

#include "type.h"

typedef struct {
    u8 features;
    u8 count;
    u8 lba_low;
    u8 lba_mid;
    u8 lba_high;
    u8 device;
    u8 command;
} HDCMD;
#define ATA_IDENTIFY		0xEC
#endif //CHP6_HD_H
