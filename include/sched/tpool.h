#ifndef INC_TPOOL_H
#define INC_TPOOL_H

#include "ds/ring.h"

#include <pthread.h>
#include <stddef.h>

struct tjob {
  void *(*proc)(void*);
  void *arg;
};

struct tpool {
  int size;
  int run;
  pthread_cond_t cond_job;
  pthread_mutex_t job_mux;
  pthread_t *pool;
  ring_t *jobs;
};

void tpool_init(struct tpool *p, uint32_t buf_len);

void tpool_del(struct tpool *p);

int tpool_job_push(struct tpool *p, void *(*proc)(void*), void *arg);

#endif
