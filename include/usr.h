//
// Created by xiazihao on 17-2-21.
//

#ifndef OSX86_USR_H
#define OSX86_USR_H

int read_hd(void *buf, int bufsize, int drive, int sector);

int write_hd(void *buf, int bufsize, int drive, int sector);
#endif //OSX86_USR_H
