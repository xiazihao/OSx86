//
// Created by xiazihao on 17-2-26.
//

#include <fs.h>
#include <lib.h>
#include <usr.h>
#include <systask.h>
#include <proto.h>
#include <global.h>

static int create_file(char *path, int flags);

static Inode *new_inode(int dev, int inode_nr, u32 start_sect);

static int attach_dir(Inode *dir_inode, Inode *file_inode, char const *file_name);

static int search(char const *file_name);

/**
 * Process open operation, the most important function in this file
 * @param flags
 * @param name_len
 * @param name_string
 * @param caller
 * @return
 */
int do_open(u32 flags, int name_len, void *name_string, u32 caller) {
    //default file descriptor -1, fd failed
    int fd = -1;
    char pathname[128];
    assert(name_len < 128);
    physic_copy(virtual2Linear(PID_FS, pathname), virtual2Linear(caller, name_string), name_len);
    printf("open file name:%s\n", pathname);
    Process *process = &process_table[caller];


    //find an empty position in process file_descriptor
    for (int j = 0; j < NR_FILE_DESCRIPTORS; ++j) {
        if (process->file_descriptor[j] == 0) {
            fd = j;
            printf("file descriptor in process:%d\n", fd);
            break;
        }
    }
    assert(fd >= 0 || fd < NR_FILE_DESCRIPTORS);
    //find an empty position in file_descriptor_table
    for (int i = 0; i < NR_DESCRIPTOR_CACHE; ++i) {
        if (file_descriptor_table[i].inode_nr == NULL) {
            process->file_descriptor[fd] = &file_descriptor_table[i];
            printf("find file descriptor position at:%d\n", i);
            break;
        }
    }
    char t[] = "test file";
    int search_result = search(t);
    if (search_result) {
        process->file_descriptor[fd]->inode_nr = search_result;
        process->file_descriptor[fd]->fd_mode = flags;
        process->file_descriptor[fd]->count = 0;
        process->file_descriptor[fd]->start = process->file_descriptor[fd]->end = 0;
        return fd;
    }
//    assert(flags == O_FLAG_CREATE);
    if (flags & O_FLAG_CREATE) {
        process->file_descriptor[fd]->inode_nr = create_file(0, flags);
        process->file_descriptor[fd]->fd_mode = flags;
        process->file_descriptor[fd]->count = 0;
        process->file_descriptor[fd]->start = process->file_descriptor[fd]->end = 0;
    }
    return fd;
}

static int create_file(char *path, int flags) {
    Inode *dir_inode;
    dir_inode = get_inode(DEV_HD, 1);
    printf("dir_start:%d\n", dir_inode->i_start_sect);
    printf("dir_imode:%x\n", dir_inode->i_mode);
    printf("dir_size:%d\n", dir_inode->i_size);
    int inode_nr = alloc_inode_map(dir_inode->i_dev);
    printf("alloc a inode bit at:%d\n", inode_nr);
    u32 free_sect_nr = alloc_sector_map(dir_inode->i_dev, 1);
    printf("alloc free sectors start at:%d\n", free_sect_nr);
    //create a new inode
    Inode *newinode = new_inode(dir_inode->i_dev, inode_nr, free_sect_nr);
    printf("create a new inode\n");
    attach_dir(dir_inode, newinode, path);
    printf("created file inode at: %d\n file at %d\n", newinode->i_num, newinode->i_start_sect);
    free_inode(dir_inode);
    free_inode(newinode);
    return inode_nr;
}

static int attach_dir(Inode *dir_inode, Inode *file_inode, char const *file_name) {
    assert(dir_inode->i_mode == INODE_MODE_DIRECTORY);
    read_hd(fsbuf, SECTOR_SIZE, 6, dir_inode->i_start_sect);
    DirEntry *dirEntry = (DirEntry *) (fsbuf);
    printf("attach dir: dir inode position:%d\n", dir_inode->i_start_sect);
    dirEntry = (DirEntry *) (&fsbuf[dir_inode->i_size]);
    dirEntry->inode_nr = file_inode->i_start_sect;
    char t[] = "test file";
    memcpy(dirEntry->name, t, sizeof(t));
    dir_inode->i_size += DIR_ENTRY_SIZE;
    write_hd(fsbuf, SECTOR_SIZE, 6, dir_inode->i_start_sect);
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
u32 alloc_sector_map(int dev, int number) {
    bool failed;
    SuperBlock *superBlock = get_super_block(dev);
    int map_start = 1 + 1 + superBlock->nr_inode_map_sects;
//    printf("start %d", superBlock_cache->nr_inode_sects);
    assert(map_start == 3);
    u32 result = 0;
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
                result = (i * (u32) SECTOR_SIZE + j) * 8 + k;
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

/**
 * Create new inode structure and write back to hd left inode in inode cache
 * @param dev
 * @param inode_nr
 * @param start_sect
 * @return
 */
static Inode *new_inode(int dev, int inode_nr, u32 start_sect) {
    Inode *new_inode = get_inode(dev, inode_nr);
    new_inode->i_mode = INODE_MODE_REGULAR;
    new_inode->i_size = 0;
    new_inode->i_start_sect = start_sect;
    new_inode->i_nr_sects = 1;

    new_inode->i_dev = dev;
    new_inode->i_alive = TRUE;
    new_inode->i_num = inode_nr;

    sync_inode(new_inode);
    return new_inode;

}


static int search(char const *file_name) {
    printf("searching file: %s\n", file_name);
    Inode *dir_inode;
    dir_inode = get_inode(DEV_HD, 1);
    read_hd(fsbuf, SECTOR_SIZE, 6, dir_inode->i_start_sect);
    DirEntry *dirEntry = (DirEntry *) (fsbuf);
    int file_number = dir_inode->i_size / DIR_ENTRY_SIZE;
    printf("%d / %d this directory have %d files\n", dir_inode->i_size, DIR_ENTRY_SIZE, file_number);
    for (int i = 0; i < file_number; ++i) {
        printf("file in director: %s\n", dirEntry->name);
        if (cmpstr(dirEntry->name, file_name)) {
            printf("find file: %s\n", file_name);
            return dirEntry->inode_nr;
        }
        dirEntry++;
    }
    return 0;

}
