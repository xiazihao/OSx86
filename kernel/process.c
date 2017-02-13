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
    if (p_proc_ready->ticks <= 0) {
        p_proc_ready->ticks = p_proc_ready->priority;
        p_proc_ready++;
        if (p_proc_ready >= process_table + NR_TASKS + NR_PROCS) {
            p_proc_ready = process_table;
        }
    }
}

//int sys_sendrec() {
//
//}