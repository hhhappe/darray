#ifndef _TIMER_H
#define _TIMER_H

#include <time.h>

inline static double nsec() {
    struct timespec tp;
//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    return tp.tv_sec*1e9 + tp.tv_nsec;
}

#endif
