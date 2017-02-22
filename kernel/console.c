//
// Created by xiazihao on 2/9/17.
//

#include <global.h>
#include "console.h"
#include "proto.h"
#include "tty.h"

static void set_cursor(unsigned int position);

static void set_video_start_addr(u32 addr);

void out_char(Console *p_console, char ch) {
    volatile u8 *p_vmem = (u8 *) (V_MEM_BASE + p_console->cursor * 2);
    switch (ch) {
        case '\n':
            //make sure that cursor is not go out of limit
            if (p_console->cursor < p_console->original_addr + p_console->v_mem_limit - SCREEN_WIDTH) {

                p_console->cursor =
                        (((p_console->cursor - p_console->original_addr) / SCREEN_WIDTH) + 1) * SCREEN_WIDTH +
                        p_console->original_addr;
            }
            break;
        case '\b':
            if (p_console->cursor > p_console->original_addr) { //make sure that cursor is not go out of limit
                p_console->cursor--;
                //clear with space
                *(p_vmem - 2) = ' ';
                *(p_vmem - 1) = DEFAULT_CHAR_COLOR;
            }
            break;
        case 9:
            //make sure that cursor is not go out of limit
            if (p_console->cursor + 4 < p_console->original_addr + p_console->v_mem_limit) {
                p_console->cursor =
                        ((p_console->cursor - p_console->original_addr) / 4 + 1) * 4 + p_console->original_addr;
            }
            break;
        default:
            //make sure thast cursor is not go out of limit
            if (p_console->cursor < p_console->original_addr + p_console->v_mem_limit - 1) {
                *p_vmem++ = ch;
                *p_vmem++ = DEFAULT_CHAR_COLOR;
                p_console->cursor++;
            }
            break;
    }
    while (p_console->cursor >= p_console->current_start_addr + SCREEN_SIZE) {
        scroll_screen(p_console, SCR_DN);
    }

//    set_cursor(p_console->cursor);
    if (&console_table[nr_current_console] == p_console) {
        set_video_start_addr(p_console->current_start_addr);
        set_cursor(p_console->cursor);
    }
}

void initScreen(TTY *p_tty) {
    int nr_tty = p_tty - tty_table;
    p_tty->p_console = console_table + nr_tty;
    int v_mem_size = V_MEM_SIZE >> 1;
    int con_v_mem_size = v_mem_size / NR_CONSOLES;
    p_tty->p_console->original_addr = nr_tty * con_v_mem_size;
    p_tty->p_console->v_mem_limit = con_v_mem_size;
    p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

    p_tty->p_console->cursor = p_tty->p_console->original_addr;
//    if (nr_tty == 0) {
//        p_tty->p_console->cursor = disp_pos / 2;
//        disp_pos = 0;
//    } else {
    out_char(p_tty->p_console, nr_tty + '0');
    out_char(p_tty->p_console, '#');
    out_char(p_tty->p_console, '\n');

//    }
    set_cursor(p_tty->p_console->cursor);
}

static void set_cursor(unsigned int position) {
    disable_int();
    out_byte(CTRC_ADDR_REG, CURSOR_H);
    out_byte(CTRC_DATA_REG, (position >> 8) & 0xFF);
    out_byte(CTRC_ADDR_REG, CURSOR_L);
    out_byte(CTRC_DATA_REG, (position) & 0xFF);
    enable_int();
}

void select_console(int nr_console) {
    if (nr_console < 0 || nr_console >= NR_CONSOLES) {
        return;
    }
    nr_current_console = nr_console;
    set_cursor(console_table[nr_console].cursor);
    set_video_start_addr(console_table[nr_console].current_start_addr);

}

static void set_video_start_addr(u32 addr) {
    disable_int();
    out_byte(CTRC_ADDR_REG, START_ADDR_H);
    out_byte(CTRC_DATA_REG, (addr >> 8) & 0xff);
    out_byte(CTRC_ADDR_REG, START_ADDR_L);
    out_byte(CTRC_DATA_REG, (addr) & 0xff);
    enable_int();
}

void scroll_screen(Console *p_console, int direction) {
    if (direction == SCR_UP) {
        if (p_console->current_start_addr > p_console->original_addr) {
            p_console->current_start_addr -= SCREEN_WIDTH;
        }
    } else if (direction == SCR_DN) {
        if (p_console->current_start_addr + SCREEN_SIZE <
            p_console->original_addr + p_console->v_mem_limit) {
            p_console->current_start_addr += SCREEN_WIDTH;
        }
    } else {
    }
    if (&console_table[nr_current_console] == p_console) {
        set_video_start_addr(p_console->current_start_addr);
        set_cursor(p_console->cursor);
    }
}