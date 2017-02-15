//
// Created by xiazihao on 2/15/17.
//

#include <process.h>
#include <proto.h>
#include <lib.h>
#include <hd.h>

#define DEVICE_REG(lba,drv,lbaHigh) ((lba << 6) | (drv <<4) | (lbaHigh &0xf) | 0xa0)
static void init_hd();

void hd_handler(int irq);

void task_hd() {
    MESSAGE msg;

}

static void init_hd() {
    u8 *pNrDrives = (u8 *) (0x475);
    printf("NrDrives:%d.\n", *pNrDrives);
    assert(*pNrDrives);
    put_irq_handler(AT_WINI_IRQ, hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
}
static void hd_identify(int drive){
    HDCMD cmd;
    cmd.device = DEVICE_REG(0,drive,0);
    cmd.command = ATA_IDENTIFY;
}
void hd_handler(int irq) {

}