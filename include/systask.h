//
// Created by xiazihao on 2/15/17.
//

#ifndef CHP6_SYSTASK_H
#define CHP6_SYSTASK_H
#define SYSGETTICKS 1
#define SYSWAIT 2

#define SYSTASKPID  1
#define HDPID       2

int wait(int millsec);

int getTicks();

#endif //CHP6_SYSTASK_H
