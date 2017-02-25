//
// Created by xiazihao on 17-2-21.
//
#include <hd.h>
#include <fs.h>
#include <usr.h>
#include <lib.h>
#include <proto.h>

u8 *fsbuf = (u8 *) 0x600000;
const int FSBUF_SIZE = 0x100000;

static void mkfs();

void task_fs() {
    SuperBlock superBlock;
    printf("\nfile system task\n");
    open_hd();
    mkfs();

    if (read_hd(&superBlock, SUPER_BLOCK_SIZE, 6, 1)) {
        printf("read error");
    }
    assert(superBlock.magic == MAGIC_V1);
    printf("\n\nnr_inode: %d", superBlock.nr_inode);
    printf("\n\nn_1st_sect:%d", superBlock.n_1st_sect);
    printf("mkfs");
    while (1) {

    }
}

static void init_fs() {

}

static void mkfs() {
    u32 bits_per_sect = SECTOR_SIZE * 8;
    PartitionInfomation boot_partition_info;
    partition_infomation(&boot_partition_info, 6);
    /**
     * Super block
     */
    SuperBlock superBlock;
    superBlock.magic = MAGIC_V1;
    superBlock.nr_inode = bits_per_sect;
    superBlock.nr_sects = boot_partition_info.size;
    superBlock.nr_inode_map_sects = 1;
    superBlock.nr_sector_map_sects = superBlock.nr_sects / bits_per_sect + 1;
    superBlock.nr_inode_sects = superBlock.nr_inode * INODE_SIZE / SECTOR_SIZE;
    superBlock.n_1st_sect =
            1 + 1 + superBlock.nr_inode_map_sects + superBlock.nr_sector_map_sects +
            superBlock.nr_inode_sects; // boot sect + super block sector

    superBlock.root_inode = 1;
    superBlock.inode_size = INODE_SIZE;
    superBlock.inode_isize_offset;
    superBlock.inode_start_offset;
    superBlock.dir_entry_size;
    superBlock.dir_entry_inode_offset;
    superBlock.dir_entry_fname_offset;
    memcpy(fsbuf, &superBlock, SUPER_BLOCK_SIZE);
    write_hd(fsbuf, SUPER_BLOCK_SIZE, 6, 1);


    printf("sector size %d\n", superBlock.nr_sects);
    printf("sector map sectors:%d\n", superBlock.nr_sector_map_sects);
    printf("inode size:%d\n", superBlock.inode_size);
    printf("inode array secotors: %d\n", superBlock.nr_inode_sects);

    /**
     * inode map
     */
    memset(fsbuf, 0, SECTOR_SIZE);
    // bit 0: reserved bit 1:root dir bit 2: dev_tty0 bit 3:dev_tty1 bit 4:dev_tty2
    for (int i = 0; i < (NR_CONSOLES + 2); ++i) {
        fsbuf[0] |= 1 << i;
    }
    assert(fsbuf[0] == 0x1f);
    write_hd(fsbuf, SECTOR_SIZE, 6, 2);
    /**
     * sector map
     */
    memset(fsbuf, 0, SECTOR_SIZE * superBlock.nr_sector_map_sects);
    // include root dir sector
    int left = superBlock.n_1st_sect;
    for (int i = 0; left; ++i) {
        if (left < 8) {
            fsbuf[i] = ~(((u8) ~0) << left);
//            printf("i:%d left:%d %x", i, left, fsbuf[i]);
            left = 0;
        } else {
            fsbuf[i] = 0xFF;
            left -= 8;
        }
    }
    int index = 0;
    for (int i = 0; i < superBlock.nr_sector_map_sects; ++i) {
        write_hd(&fsbuf[index], SECTOR_SIZE, 6, 3 + i);
        index += SECTOR_SIZE;
        printf("sector map:%d", 3 + i);
    }
    /**
     * inode array
     */
    printf("nr inode sects:%d", superBlock.nr_inode_sects);
    memset(fsbuf, 1, SECTOR_SIZE * superBlock.nr_inode_sects);
    //root inode
    Inode *inode = (Inode *) fsbuf;
    inode->i_mode = 1;
    inode->i_start_sect = superBlock.n_1st_sect; // root directory
    inode->i_nr_sects = 0;
    inode->i_size = (1 + NR_CONSOLES) * DIR_ENTRY_SIZE;// . and tty0-2, 4 files

    inode++;
    for (int i = 0; i < NR_CONSOLES; ++i) {
        inode->i_mode = INODE_MODE_CHAR;
        inode->i_size = 0;
        inode->i_nr_sects = 0;
    }

    printf("inode array sector start:%d\n", 3 + superBlock.nr_sector_map_sects);
    index = 0;
    printf("Init inode array ");
    for (int i = 0; i < superBlock.nr_inode_sects; ++i) {
        write_hd(&fsbuf[index], SECTOR_SIZE, 6, 3 + superBlock.nr_sector_map_sects + i);
        index += SECTOR_SIZE;
        printf(". ");
    }
    /**
     * root directory
     */
    printf("1st_sect:%x\n", superBlock.n_1st_sect);
    memset(fsbuf, 0, SECTOR_SIZE);
    DirEntry *dirEntry = (DirEntry *) fsbuf;
    //
    strcpy(dirEntry->name, ".");
    dirEntry->inode_nr = 1;//root directory itself
    //
    dirEntry++;
    strcpy(dirEntry->name, "tty 0");
    dirEntry->inode_nr = 2;
    //
    dirEntry++;
    strcpy(dirEntry->name, "tty 1");
    dirEntry->inode_nr = 3;
    //
    dirEntry++;
    strcpy(dirEntry->name, "tty 2");
    dirEntry->inode_nr = 4;
    write_hd(fsbuf, SECTOR_SIZE, 6, superBlock.n_1st_sect);


}