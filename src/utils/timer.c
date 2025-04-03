#include "utils/timer.h"

size_t clock_timesince_nsec(clockid_t clock_id, struct timespec *tp)
{
  struct timespec up;
  clock_gettime(clock_id, &up);

  up.tv_sec -= tp->tv_sec;
  up.tv_nsec -= tp->tv_nsec;

  if (up.tv_nsec < 0) {
    up.tv_nsec = 1e9 + up.tv_nsec;
    up.tv_sec -= 1;
  }

  return 1e9*up.tv_sec + up.tv_nsec;
}
