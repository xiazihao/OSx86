//
// Created by xiazihao on 2/8/17.
//

#ifndef CHP6_CONSOLE_H
#define CHP6_CONSOLE_H

#include "type.h"

typedef struct s_tty TTY;
typedef struct s_console {
    unsigned int current_start_addr; //
    unsigned int original_addr;//if initial and won't be changed
    unsigned int v_mem_limit;
    unsigned int cursor;//index of cursor, start from address of video section
} Console;

/**
 * Default color of printed character, 0x0F black
 */
#define DEFAULT_CHAR_COLOR 0x0f//black

/**
 * Screen width
 */

#define SCREEN_WIDTH 80

/**
 * Screen side = screen width * screen height
 */
#define SCREEN_SIZE (80*25)

/**
 * Screen operation code
 */
#define SCR_UP    1    /* scroll forward */
#define SCR_DN    -1    /* scroll backward */

void initScreen(TTY *p_tty);

void out_char(Console *p_console, char ch);

void select_console(int nr_console);

void scroll_screen(Console *p_console, int direction);

#endif //CHP6_CONSOLE_H
