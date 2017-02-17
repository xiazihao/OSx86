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
#define ATA_IDENTIFY        0xEC

#define DEV_OPEN    1

//register
#define REG_DATA        0x1F0
#define REG_FEATURE     0x1F1
#define REG_ERROR       REG_FEATURE
#define REG_SECTORCOUNT 0x1F2
#define REG_LBALOW      0x1F3
#define REG_LBAMID      0x1F4
#define REG_LBAHIGH     0x1F5
#define REG_DEVICE      0x1F6
#define REG_STATUS      0x1F7
#define STATUS_BUSY     0x80

#define REG_CMD         REG_STATUS

#define REG_DEVICECONTROL   0x3F6
#define REG_ALTSTATUS   REG_DEVICECONTROL


#define MAX_DRIVERS     2
#define NR_PART_PER_DRIVE   4
#define NR_SUB_PER_PART     16
#define NR_SUB_PER_DRIVE    (NR_PART_PER_DRIVE + NR_SUB_PER_PART)
#define NR_PRIM_PER_DRIVE   (NR_PART_PER_DRIVE + 1)
#define MAX_PRIM    (MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)
#define MAX_SUBPARTITIONS   (NR_SUB_PER_DRIVE * MAX_DRIVES)
#endif //CHP6_HD_H
