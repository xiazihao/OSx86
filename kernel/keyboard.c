#include <tty.h>
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proto.h"
#include "keyboard.h"
#include "keymap.h"


static KB_INPUT kb_input;
static int code_with_EO = 0;
static int shift_l;
static int shift_r;
static int alt_l;
static int alt_r;
static int ctrl_l;
static int ctrl_r;
static int caps_lock;
static int num_lock;
static int scroll_lock;
static int column;

static u8 get_byte_from_kbuf();

void keyboard_handler(int irq) {
    u8 scan_code = in_byte(0x60);
    if (kb_input.count < KB_IN_BYTES) {
        *(kb_input.p_head) = scan_code;
        kb_input.p_head++;
        if (kb_input.p_head == kb_input.buf + KB_IN_BYTES) {
            kb_input.p_head = kb_input.buf;
        }
        kb_input.count++;
    }
}

void init_keyboard() {
    kb_input.count = 0;
    kb_input.p_head = kb_input.p_tail = kb_input.buf;
    putIrqHandler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
    code_with_EO = 0;
    shift_l = 0;
    shift_r = 0;
    alt_l = 0;
    alt_r = 0;
    ctrl_l = 0;
    ctrl_r = 0;
    caps_lock = 0;
    num_lock = 0;
    scroll_lock = 0;
    column = 0;

}

static u8 get_byte_from_kbuf() {
    u8 scan_code;
    while (kb_input.count <= 0) {}
    disable_int();
    scan_code = *(kb_input.p_tail);
    kb_input.p_tail++;
    if (kb_input.p_tail == kb_input.buf + KB_IN_BYTES) {
        kb_input.p_tail = kb_input.buf;
    }
    kb_input.count--;
    enable_int();
    return scan_code;
}

void keyboard_read(TTY *p_tty) {
    u8 scan_code;
    int make;
    u32 key = 0;
    u32 *keyrow;
    if (kb_input.count > 0) {
        code_with_EO = 0;
        scan_code = get_byte_from_kbuf();
        if (scan_code == 0xe1) {
            int i;
            u8 pausebrk_scode[] = {0xe1, 0x1d, 0x45, 0xe1, 0x9d, 0xc5};
            int is_pausebreak = 1;
            for (i = 0; i < 6; i++) {
                if (get_byte_from_kbuf() != pausebrk_scode[i]) {
                    is_pausebreak = 0;
                    break;
                }
            }
            if (is_pausebreak) {
                key = PAUSEBREAK;
            }

        } else if (scan_code == 0xe0) {
            scan_code = get_byte_from_kbuf();
            //Print Screen press
            if (scan_code == 0x2a) {
                if (get_byte_from_kbuf() == 0xe0) {
                    if (get_byte_from_kbuf() == 0x37) {
                        key = PRINTSCREEN;
                        make = 1;
                    }
                }
            }
            //Print Sceen unpress
            if (scan_code == 0xb7) {
                if (get_byte_from_kbuf() == 0xe0) {
                    if (get_byte_from_kbuf() == 0xaa) {
                        key = PRINTSCREEN;
                        make = 0;
                    }
                }
            }
            if (key == 0) {
                code_with_EO = 1;
            }
        }
        if ((key != PAUSEBREAK && key != PRINTSCREEN)) {
            make = (scan_code & FLAG_BREAK ? 0 : 1);
            keyrow = &keymap[(scan_code & 0x7f) * MAP_COLS];
            column = 0;
            if (shift_l || shift_r) {
                column = 1;
            }
            if (code_with_EO) {
                column = 2;
                code_with_EO = 0;
            }
            key = keyrow[column];
            switch (key) {
                case SHIFT_L:
                    shift_l = make;
                    break;
                case SHIFT_R:
                    shift_r = make;
                    break;
                case CTRL_L:
                    ctrl_l = make;
                    break;
                case CTRL_R:
                    ctrl_r = make;
                    break;
                case ALT_L:
                    alt_l = make;
                    break;
                case ALT_R:
                    alt_r = make;
                    break;
                default:
                    break;
            }
            if (make) {//if it is break code:ignore
                key |= shift_l ? FLAG_SHIFT_L : 0;
                key |= shift_r ? FLAG_SHIFT_R : 0;
                key |= ctrl_r ? FLAG_CTRL_L : 0;
                key |= ctrl_l ? FLAG_CTRL_L : 0;
                key |= alt_l ? FLAG_ALT_L : 0;
                key |= alt_r ? FLAG_ALT_R : 0;
                in_process(p_tty, key);
            }
        }
    }
}


