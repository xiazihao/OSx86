//
// Created by xiazihao on 2/15/17.
//

#include <process.h>
#include <proto.h>
#include <lib.h>
#include <hd.h>
#include <systask.h>
#include <fs.h>
#include <global.h>

#define    MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba) << 6) |        \
                          ((drv) << 4) |        \
                          (lba_highest & 0xF) | 0xA0)

#define SECTOR_SIZE     512
static u8 hd_status;
static u8 hdbuf[SECTOR_SIZE * 2];
static HdInformation hdInfo[1];

static void hdOpen(int device);

static void partition();

static void initHardDrive();

static int waitFor(int mask, int value, int timeout);

void print_identify_info(u16 *hdinfo);

static void interruptWait();

static void commandOut(HdCmd *hdcmd);

void getPartTable(int drive, int sect_nr, PartitionEntry *partEntry, int num);

void hd_handler(int irq);

static void printPartitionTable();

/**
 * Init hard drive: get drive nubmer; set hard drive interrupt handler; enable hard drive handler;
 * init hard drive information structure
 * It should be firstly call before every operation about hard drive
 */
static void initHardDrive() {
    u8 *pNrDrives = (u8 *) (0x475);
    printf("NrDrives:%d.\n", *pNrDrives);
    assert(*pNrDrives);
    putIrqHandler(AT_WINI_IRQ, hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
    //init hard drive information structure
    memset(hdInfo, 0, sizeof(HdInformation));
}

static void hd_identify(int drive) {
    HdCmd cmd;
    cmd.device = MAKE_DEVICE_REG(0, drive, 0);
    cmd.command = ATA_IDENTIFY;
    commandOut(&cmd);
    interruptWait();
    port_read(REG_DATA, hdbuf, SECTOR_SIZE);
    print_identify_info((u16 *) hdbuf);
//    partition();
//    printPartitionTable();
}

void print_identify_info(u16 *hdinfo) {
    int i, k;
    char s[64];

    struct iden_info_ascii {
        int idx;
        int len;
        char *desc;
    } iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
                 {27, 40, "HD Model"} /* Model number in ASCII */ };

    for (k = 0; k < sizeof(iinfo) / sizeof(iinfo[0]); k++) {
        char *p = (char *) &hdinfo[iinfo[k].idx];
        for (i = 0; i < iinfo[k].len / 2; i++) {
            s[i * 2 + 1] = *p++;
            s[i * 2] = *p++;
        }
        s[i * 2] = 0;
        printf("%s: %s\n", iinfo[k].desc, s);
    }

    int capabilities = hdinfo[49];
    printf("LBA supported: %s\n",
           (capabilities & 0x0200) ? "Yes" : "No");

    int cmd_set_supported = hdinfo[83];
    printf("LBA48 supported: %s\n",
           (cmd_set_supported & 0x0400) ? "Yes" : "No");

    int sectors = ((int) hdinfo[61] << 16) + hdinfo[60];
    printf("HD size: %dMB\n", sectors * 512 / 1000000);
}

/**
 * Output command to hard drive controller
 * @param hdcmd  command struct
 */
static void commandOut(HdCmd *hdcmd) {
    if (waitFor(STATUS_BUSY, 0, 10000)) {
        printf("hd error");
        while (1);
    }
    //firstly write 'device control' to enable interrupt,the write 'feature'
    //, 'sector count','lba low','lba mid','lba high','lba device',finally
    //write 'command' to launch hd
    out_byte(REG_DEVICECONTROL, 0);
    out_byte(REG_FEATURE, hdcmd->features);
    out_byte(REG_SECTORCOUNT, hdcmd->count);
    out_byte(REG_LBALOW, hdcmd->lba_low);
    out_byte(REG_LBAMID, hdcmd->lba_mid);
    out_byte(REG_LBAHIGH, hdcmd->lba_high);
    out_byte(REG_DEVICE, hdcmd->device);
    //
    out_byte(REG_CMD, hdcmd->command);

}

/**
 * Block until specific bit equals to given value
 * @param mask bit mask of the bit which is concerned
 * @param value bit value we expect, 0 or 1
 * @param timeout timeout time
 * @return 0:success 1:timeout
 */
static int waitFor(int mask, int value, int timeout) {
    int t = getTicks();
    assert(value == 0 || value == 1);
    while ((getTicks() - t) * 1000 / HZ < timeout) {
        if ((in_byte(REG_STATUS) & mask) == value) {
            return 0;
        }
    }
    return 1;
}

/**
 * Block until hard drive intterupt occur
 */
static void interruptWait() {
    receivemessage(INFORM, ANY, NULL);
    while (receivemessage(INT, ANY, NULL));
}

void task_hd() {
    Message msg;
    initHardDrive();
    receivemessage(INFORM, ANY, &msg);
    while (TRUE) {
        while (receivemessage(RECEIVE, ANY, &msg));
        switch (msg.type) {
            case DEV_OPEN:
//                printf("get open");
                hdOpen(0);
//                while (1);
                break;
        }
    }
}

static void printPartitionTable() {
    Process *p = &process_table[0];
    printf("%s", p->name);
    for (int i = 0; i < 10; ++i) {
        if (hdInfo[0].partitionInformation[i].partitionBasic.sysId == 0) {
            continue;
        }
        printf("Partition: %s, sysId: %x, active%x\n", hdInfo[0].partitionInformation[i].name,
               hdInfo[0].partitionInformation[i].partitionBasic.sysId,
               hdInfo[0].partitionInformation[i].partitionBasic.status);
        printf("start: %x,count: %x", hdInfo[0].partitionInformation[i].partitionBasic.startSectorFromStart,
               hdInfo[0].partitionInformation[i].partitionBasic.sectorCount);
        printf("\n");
    }
}

/**
 * Scan whole hard drive, get partition
 */
static void partition() {
    PartitionEntry partitionBuf[4];
    getPartTable(0, 0, partitionBuf, NR_PART_PER_DRIVE);
    memcpy(&hdInfo[0].partitionInformation[0].partitionBasic, &partitionBuf[0], sizeof(PartitionEntry));
    memcpy(&hdInfo[0].partitionInformation[1].partitionBasic, &partitionBuf[1], sizeof(PartitionEntry));
    memcpy(&hdInfo[0].partitionInformation[2].partitionBasic, &partitionBuf[2], sizeof(PartitionEntry));
    memcpy(&hdInfo[0].partitionInformation[3].partitionBasic, &partitionBuf[3], sizeof(PartitionEntry));
    int temp = NR_PART_PER_DRIVE;
    for (int i = 0; i < 4; ++i) {
        if (hdInfo[0].partitionInformation[i].partitionBasic.sysId != 0) {
            hdInfo[0].partitionInformation[i].name[0] = 'h';
            hdInfo[0].partitionInformation[i].name[1] = 'd';
            hdInfo[0].partitionInformation[i].name[2] = (char) ('1' + i);
            hdInfo[0].partitionInformation[i].name[3] = 0;
        }
        if (hdInfo[0].partitionInformation[i].partitionBasic.sysId == 5) {
            char idx = 'a';
            u32 baseSector = hdInfo[0].partitionInformation[i].partitionBasic.startSectorFromStart;
            u32 startSector = hdInfo[0].partitionInformation[i].partitionBasic.startSectorFromStart;
            do {
                getPartTable(0, startSector, partitionBuf, 2);
                memcpy(&hdInfo[0].partitionInformation[temp].partitionBasic, partitionBuf, sizeof(PartitionEntry));

                hdInfo[0].partitionInformation[temp].name[0] = 'h';
                hdInfo[0].partitionInformation[temp].name[1] = 'd';
                hdInfo[0].partitionInformation[temp].name[2] = (char) ('1' + i);
                hdInfo[0].partitionInformation[temp].name[3] = idx;
                hdInfo[0].partitionInformation[temp].name[4] = 0;

                startSector = partitionBuf[1].startSectorFromStart + baseSector;
                temp++;
                idx++;
            } while (partitionBuf[1].sysId == 5);

        }
    }
}

/**
 * Get partition information by read hard drive sector
 * @param drive drive index, 0: master hard drive
 * @param sect_nr sector number
 * @param partEntry address which receive partition information
 * @param num max items of partition information
 */
void getPartTable(int drive, int sect_nr, PartitionEntry *partEntry, int num) {
    HdCmd hdcmd;
    hdcmd.features = 0;
    hdcmd.count = 1;
    hdcmd.lba_low = (u8) (sect_nr & 0xff);
    hdcmd.lba_mid = (u8) ((sect_nr >> 8) & 0xff);
    hdcmd.lba_high = (u8) ((sect_nr >> 16) & 0xff);
    hdcmd.device = (u8) (MAKE_DEVICE_REG(1, drive, (sect_nr >> 24) & 0xf));
    hdcmd.command = ATA_READ;
    commandOut(&hdcmd);
    interruptWait();
    port_read(REG_DATA, hdbuf, SECTOR_SIZE);
    memcpy(partEntry, hdbuf + PARTITION_TABLE_OFFSET, sizeof(PartitionEntry) * num);
}

/**
 *
 * @param device
 */
static void hdOpen(int device) {
    assert(device == 0);
    hd_identify(device);
    if (hdInfo[device].open_count == 0) {
        partition();
        printPartitionTable();
    }
    hdInfo[device].open_count++;
}

/**
 * Hard drive interrupt handler
 * @param irq interrupt index
 */
void hd_handler(int irq) {
    hd_status = in_byte(REG_STATUS);
    informInterrupt(PID_HD);
//    while (1);
}