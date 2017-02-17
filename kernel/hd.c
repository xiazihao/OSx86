//
// Created by xiazihao on 2/15/17.
//

#include <process.h>
#include <proto.h>
#include <lib.h>
#include <hd.h>
#include <systask.h>

#define DEVICE_REG(lba, drv, lbaHigh) ((lba << 6) | (drv <<4) | (lbaHigh &0xf) | 0xa0)

#define SECTOR_SIZE     512
static u8 hd_status;
static u8 hdbuf[SECTOR_SIZE * 2];

static void init_hd();

static int waitfor(int mask, int value, int timeout);

void print_identify_info(u16 *hdinfo);

static void interrupt_wait();

static void hd_cmd_out(HDCMD *hdcmd);

void hd_handler(int irq);


static void init_hd() {
    u8 *pNrDrives = (u8 *) (0x475);
    printf("NrDrives:%d.\n", *pNrDrives);
    assert(*pNrDrives);
    putIrqHandler(AT_WINI_IRQ, hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
}

static void hd_identify(int drive) {
    HDCMD cmd;
    cmd.device = DEVICE_REG(0, drive, 0);
    cmd.command = ATA_IDENTIFY;
//    receivemessage()
    hd_cmd_out(&cmd);
    interrupt_wait();
    port_read(REG_DATA, hdbuf, SECTOR_SIZE);
    print_identify_info((u16 *) hdbuf);

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

static void hd_cmd_out(HDCMD *hdcmd) {
    if (waitfor(STATUS_BUSY, 0, 10000)) {
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
    out_byte(REG_CMD, hdcmd->device);

}

static int waitfor(int mask, int value, int timeout) {
    int t = getTicks();
    while ((getTicks() - t) * 1000 / HZ < timeout) {
        if ((in_byte(REG_STATUS) & mask) == value) {
            return 0;
        }
    }
    return 1;
}

static void interrupt_wait() {
    receivemessage(INFORM, ANY, NULL);
    while (receivemessage(INT, ANY, NULL));
}

void task_hd() {
    Message msg;
    init_hd();
    receivemessage(INFORM, ANY, &msg);
    while (TRUE) {
        while (receivemessage(RECEIVE, ANY, &msg));
        switch (msg.type) {
            case DEV_OPEN:
//                printf("get open");
                hd_identify(0);
                while (1);
                break;
        }
    }
}

void hd_handler(int irq) {
    hd_status = in_byte(REG_STATUS);
    informInterrupt(HDPID);
//    while (1);
}