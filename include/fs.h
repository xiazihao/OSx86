//
// Created by xiazihao on 2/17/17.
//

#ifndef CHP6_FS_H
#define CHP6_FS_H

#include "hd.h"

typedef struct {
    u32 driver_nr;
} DevDriverMap;


#define NO_DEV  0
#define DEV_FLOPPY      1
#define DEV_CDROM       2
#define DEV_HD          3
#define DEV_CHAR_TTY    4
#endif //CHP6_FS_H
