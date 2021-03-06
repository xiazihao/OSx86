//
// Created by xiazihao on 2/17/17.
//

#ifndef CHP6_FS_H
#define CHP6_FS_H

#include "hd.h"
/**
 *
 */
#define MAGIC_V1    0x222
typedef struct s_super_block {
    u32 magic;                      //magic number
    u32 nr_inode;                   //the number of inode
    u32 nr_sects;                   //the number of sectors in this partition
    u32 nr_inode_map_sects;         //the number of inode map sector
    u32 nr_sector_map_sects;        //the number of sector map sector
    u32 n_1st_sect;                 //the index of the first data sector
    u32 nr_inode_sects;             //the number of inode sector
    u32 root_inode;                 //the number of root inode
    u32 inode_size;                 //inode size
    u32 inode_isize_offset;
    u32 inode_start_offset;
    u32 dir_entry_size;
    u32 dir_entry_inode_offset;
    u32 dir_entry_fname_offset;
    /**
     * Below elements only exist in memory not store in hard drive
     */
    int super_block_device;
} SuperBlock;

#define SUPER_BLOCK_SIZE (sizeof(SuperBlock) - sizeof(int))

typedef struct s_inode {
    u32 i_mode;
    u32 i_size;
    u32 i_start_sect;
    u32 i_nr_sects;
    u8 _unused[16];
    /**
     * Below elements only exist in memory not store in hard drive
     */
    int i_dev;
    int i_count;
    int i_num;
} Inode;

#define INODE_SIZE (sizeof(Inode) - 3*sizeof(int))

#define MAX_FILE_NAME_LENGTH    12

typedef struct s_dir_entry {
    char name[MAX_FILE_NAME_LENGTH];
    int inode_nr;
} DirEntry;

#define DIR_ENTRY_SIZE  sizeof(DirEntry);

typedef struct s_file_descriptor {
    u32 fd_mode;
    int fd_position;
    Inode *fd_inode;
} FileDescriptor;

typedef struct {
    u32 driver_nr;
} DevDriverMap;
/**
 * Major device number( see dd_map[] @ global.c)
 */
#define NO_DEV          0
#define DEV_FLOPPY      1
#define DEV_CDROM       2
#define DEV_HD          3
#define DEV_CHAR_TTY    4

#define MAJOR_SHIFT     8
#define MAKE_DEV(major, substitue) ((major << MAJOR_SHIFT) | substitue)

#define MAJOR(x)        ((x >> MAJOR_SHIFT) & 0xFF)
#define MINOR(x)        (x &~(~0>>MAJOR_SHIFT<<MAJOR_SHIFT))

/**
 * inode mode
 */
#define INODE_MODE_DIRECTORY   0x00010000
#define INODE_MODE_CHAR        0x00020000
#endif //CHP6_FS_H
