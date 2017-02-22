#include <systask.h>
#include <usr.h>
#include "type.h"
#include "const.h"
#include "process.h"
#include "global.h"
#include "proto.h"

/**
 * Clock interrupt handler
 * @param irq interrupt index of 8259
 */
void clock_handler(int irq) {
    ticks++;
    p_proc_ready->ticks--;
    if (k_reenter != 0) {
        return;
    }
    if (p_proc_ready->ticks > 0) {
        return;
    }
    schedule();
}

void init_clock() {
    out_byte(TIME_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8) (TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8) ((TIMER_FREQ / HZ) >> 8));
    putIrqHandler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
}

void milli_delay(int milli_sec) {
    int t = get_ticks();
    int curent;
    do {
        curent = get_ticks();
    } while (((curent - t) * 1000 / HZ) < milli_sec);
}