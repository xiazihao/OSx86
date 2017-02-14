#include <lib.h>
#include "const.h"
#include "global.h"
#include "protect.h"
#include "proto.h"
#include "type.h"
#include "process.h"

public int sys_get_ticks() {
    return ticks;
}

public void schedule() {
    if (p_proc_ready->ticks > 0) {
        return;
    }
    p_proc_ready->ticks = p_proc_ready->priority;
    do {
        p_proc_ready++;
        if (p_proc_ready >= process_table + NR_TASKS + NR_PROCS) {
            p_proc_ready = process_table;
            assert(p_proc_ready[NR_TASKS + NR_PROCS - 1].status == RUNNABLE);
        }
        if (p_proc_ready->status == RUNNABLE) {
            break;
        }
    } while (TRUE);
}

void testA() {
    while (1) {
//        printf("%s %d ", "A", get_ticks());
//        assert(1 > 2);
        milli_delay(10000);
    }
}

void testB() {
    while (1) {
//        disp_str("B");
        milli_delay(10000);
    }
}

int sys_sendmessage(PROCESS *process, int function, int dest, MESSAGE *message){

}