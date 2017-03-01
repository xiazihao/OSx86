//
// Created by xiazihao on 17-2-26.
//

#include <fs.h>
#include <lib.h>
#include <usr.h>
#include <systask.h>
#include <proto.h>

int do_open(int flags, int name_len, void *name_string, u32 caller) {
    int fd = -1;
    char pathname[128];
    assert(name_len < 128);
    physic_copy(virtual2Linear(PID_FS, pathname), virtual2Linear(caller, name_string), name_len);
    printf("%s", pathname);
}

/**
 *
 * @param dev
 * @return
 */
int alloc_inode_map(int dev) {
    assert(dev == DEV_HD);
    int nr_inode = 0;
    int inode_map_start_sector = 1 + 1;//1 boot sector 2 super block
    SuperBlock *superBlock = get_super_block(dev);
    assert(superBlock->magic == MAGIC_V1);
    for (int i = 0; i < superBlock->nr_inode_map_sects; ++i) {
        read_hd(fsbuf, SECTOR_SIZE, 6, inode_map_start_sector + i);
        for (int j = 0; j < SECTOR_SIZE; ++j) {
            if (fsbuf[j] == 0xFF) {
                continue;
            }
            int k;
            for (k = 0; ((fsbuf[j] >> k) & 1) != 0; ++k) {
                //nop
            }
            nr_inode = (i * SECTOR_SIZE + j) * 8 + k;
            fsbuf[j] |= (1 << k);
            write_hd(fsbuf, SECTOR_SIZE, 6, inode_map_start_sector + i);
            break;

        }
    }
    return nr_inode;

}

/**
 *
 * @param dev
 * @param number
 * @return
 */
int alloc_sector_map(int dev, int number) {
    bool failed = FALSE;
    SuperBlock *superBlock = get_super_block(dev);
    int map_start = 1 + 1 + superBlock->nr_inode_map_sects;
//    printf("start %d", superBlock_cache->nr_inode_sects);
    assert(map_start == 3);
    int result = 0;
    int number_left = number;
    //read sector map sectors
    for (int i = 0; i < superBlock->nr_sector_map_sects; ++i) {
        read_hd(fsbuf, SECTOR_SIZE, 6, map_start + i);
        for (int j = 0; j < SECTOR_SIZE; ++j) {
            if (fsbuf[j] == 0xFF) {
                continue;
            }

            int k;//point to zero bit
            number_left = number;
            failed = FALSE;
            for (k = 0; ((fsbuf[j] >> k) & 1) != 0; ++k) {
                //nop
                number_left -= 8 - k;
            }
            //get enough sectors

            for (int l = 0; number_left > 0; ++l) {
                if (j + l == SECTOR_SIZE - 1) {
                    failed = TRUE;
                    break;
                }
                if (fsbuf[j + l] == 0xFF) {
                    number_left -= 8;
                } else {
                    failed = TRUE;
                    break;
                }
            }
            if (failed == FALSE) {
                result = (i * SECTOR_SIZE + j) * 8 + k;
                number_left = number;
                int l = 0;
                int m = k;
                do {
                    fsbuf[j + l] |= (1 << (m));
                    number_left--;
                    m++;
                    if (m > 7) {
                        l++;
                        m = 0;
                    }
                } while (number_left > 0);
                fsbuf[j] |= (1 << k);
                write_hd(fsbuf, SECTOR_SIZE, 6, map_start + i);
                return result;
            }

        }

        return result;
    }
}

/**
 *
 * @param dev
 * @return
 */
SuperBlock *get_super_block(int dev) {
    for (int i = 0; i < 10; ++i) {
        if (superBlock_cache[i].super_block_device == dev) {
            return &superBlock_cache[i];
        }
    }
    assert(0);
}

static Inode *new_inode(int dev, int inode_nr, u32 start_sect) {
    Inode *new_inode = get_inode(dev, inode_nr);
    new_inode->i_mode = INODE_MODE_REGULAR;
    new_inode->i_size = 0;
    new_inode->i_start_sect = start_sect;
    new_inode->i_nr_sects = 1;

    new_inode->i_dev = dev;
    new_inode->i_count = 1;
    new_inode->i_num = inode_nr;

    sync_inode(new_inode);
    return new_inode;

}