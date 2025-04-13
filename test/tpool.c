#include "tpool.h"
#include "utils/timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*--- HELPERS ---*/

struct counter {
  int n;
  pthread_mutex_t mux;
};

void *job(void* arg);

/*--- INTEGRATION TESTS --- */

int tpool_init_push_del_test(int n);

int main(int argc, char** argv)
{
  int passed, i, n = argc > 1 ? atoi(argv[1]) : 1;

  if (n < 1)
    n = 1;
  else if (n > 10000)
    n = 10000;

  for (i = 0; i < n; ++i) {
    passed =
      tpool_init_push_del_test(1) &&
      tpool_init_push_del_test(10) &&
      tpool_init_push_del_test(100) &&
      tpool_init_push_del_test(1000) &&
      tpool_init_push_del_test(10000);
  }

  printf(passed ? "tpool tests passed\n" : "tpool tests failed\n");
    
  return 0;
}

/*--- INTEGRATION TESTS ---*/

int tpool_init_push_del_test(int n)
{
  int i;
  struct tpool p;
  struct timespec t;
  size_t ns;
  struct counter c = { 0 };
  
  tpool_init(&p, 1024);
  pthread_mutex_init(&c.mux, NULL);

  clock_gettime(CLOCK_REALTIME, &t);
  
  for (i = 0; i < n; ++i)
    while (tpool_job_push(&p, job, &c));

  tpool_del(&p);

  ns = clock_timesince_nsec(CLOCK_REALTIME, &t);

  printf("%d tpool jobs - %.3f ms\n", c.n, (ns / 1e6));
  return c.n == n;
}

/*--- HELPERS ---*/

void *job(void *arg)
{
  struct counter *c = arg;
  pthread_mutex_lock(&c->mux);
  c->n += 1;
  pthread_mutex_unlock(&c->mux);
  return arg;
}

