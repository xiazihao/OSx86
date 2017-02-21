//
// Created by xiazihao on 2/15/17.
//

#ifndef CHP6_HD_H
#define CHP6_HD_H

#include "type.h"

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
} PartitionEntry;

typedef struct {
    u8 features;
    u8 count;
    u8 lba_low;
    u8 lba_mid;
    u8 lba_high;
    u8 device;
    u8 command;
} HdCmd;
typedef PartitionEntry PartitionBasic;
typedef struct {
    PartitionBasic partitionBasic;
    char name[5];
} PartitionExtent;
typedef struct {
    PartitionExtent partitionInformation[16];
    int open_count;
} HdInformation;

#define SECTOR_SIZE     512

#define ATA_IDENTIFY        0xEC
#define ATA_READ            0x20
#define ATA_WRITE		    0x30

#define DEV_OPEN    1
#define DEV_CLOSE   2
#define DEV_READ    3
#define DEV_WRITE   4

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
#define STATUS_DRDY     1<<6
#define STATUS_DRQ      0x08//data request

#define TIMEOUT         100000

#define REG_CMD         REG_STATUS

#define REG_DEVICECONTROL   0x3F6
#define REG_ALTSTATUS   REG_DEVICECONTROL

/**
 * Max accepted hard drive number
 */
#define MAX_DRIVERS     2

/**
 * Max number of primary part per drive
 */
#define NR_PRIMARY_PART_PER_DRIVE   4

/**
 * Max logical part per primary part
 */
#define NR_LOGICAL_PART_PER_PART     16

/**
 * Max number of logical per drive
 */
#define NR_LOGICAL_PART_PER_DRIVE    (NR_PRIMARY_PART_PER_DRIVE * NR_LOGICAL_PART_PER_PART)

/**
 * Total number of primary part per drive, 0 is the whole drive
 */
#define NR_PRIMARY_PER_DRIVE   (NR_PRIMARY_PART_PER_DRIVE + 1)

/**
 * The index of primary base of all of drive, count begin with 0
 */
#define MAX_PRIMARY    (MAX_DRIVES * NR_PRIMARY_PER_DRIVE - 1)

/**
 * Max number of logical part of all of drive
 */
#define MAX_SUBPARTITIONS   (NR_LOGICAL_PART_PER_DRIVE * MAX_DRIVES)


#define    PARTITION_TABLE_OFFSET    0x1BE
#endif //CHP6_HD_H
