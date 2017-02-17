#include <systask.h>
#include "type.h"
#include "const.h"
#include "process.h"
#include "global.h"
#include "proto.h"

void clockHandler(int irq) {
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

void initClock() {
    out_byte(TIME_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8) (TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8) ((TIMER_FREQ / HZ) >> 8));
    putIrqHandler(CLOCK_IRQ, clockHandler);
    enable_irq(CLOCK_IRQ);
}

void milli_delay(int milli_sec) {
    int t = getTicks();
    int curent;
    do {
        curent = getTicks();
    } while (((curent - t) * 1000 / HZ) < milli_sec);
}