#ifndef TIMER_H
#define TIMER_H

#include <time.h>

static inline double get_time(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return -1.0;
    return ts.tv_sec + 1.0e-9 * ts.tv_nsec;
}

#endif
