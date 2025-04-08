#ifndef INC_TPOOL_H
#define INC_TPOOL_H

#include "ds/ring.h"

#include <pthread.h>

struct tjob {
  void *(*proc)(void*);
  void *arg;
};

struct tpool {
  int size;
  int run;
  pthread_cond_t *cond_job;
  pthread_mutex_t *job_mux;
  pthread_t *pool;
  ring_t *jobs;
};

void tpool_init(struct tpool *p);

void tpool_del(struct tpool *p);

void tpool_job_push(void (*job)(void*), void *arg);

#endif
