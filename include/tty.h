//
// Created by xiazihao on 2/8/17.
//

#ifndef CHP6_TTY_H
#define CHP6_TTY_H

#include "type.h"
#include "console.h"
#include "process.h"
#define  TTY_IN_BYTES 256
//typedef struct s_console;
typedef struct s_tty {
    u32 in_buf[TTY_IN_BYTES];
    u32 *p_inbuf_head;
    u32 *p_inbuf_tail;
    int inbuf_count;
    struct s_console *p_console;
} TTY;

void inProcess(TTY *p_tty, u32 key);

void ttyWrite(TTY *p_tty, char *buf, int len);

#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

#endif //CHP6_TTY_H
