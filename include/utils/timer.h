#ifndef INC_UTIL_TIME_H
#define INC_UTIL_TIME_H

#include <time.h>

size_t clock_timesince_nsec(clockid_t clock_id, struct timespec *tp);

#endif
