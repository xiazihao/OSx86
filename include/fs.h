//
// Created by xiazihao on 2/17/17.
//

#ifndef CHP6_FS_H
#define CHP6_FS_H

#include "hd.h"
/**
 *
 */
#define MAGIC_V1    0x111

/**
 *
 */
typedef struct {
    u32 magic;
    u32 nr_inode;
    u32 nr_sects;
    u32 nr_imap_sects;
    u32 nr_smap_sects;
    u32 n_1st_sect;
} SuperBlock;

typedef struct {
    u32 driver_nr;
} DevDriverMap;
/**
 * Device index
 */
#define NO_DEV          0
#define DEV_FLOPPY      1
#define DEV_CDROM       2
#define DEV_HD          3
#define DEV_CHAR_TTY    4
#endif //CHP6_FS_H
