#ifndef INC_TPOOL_H
#define INC_TPOOL_H

#include <pthread.h>

struct tpool {
  pthread_t *pool;
  int size;
};

void tpool_init(struct tpool *p);

void tpool_del(struct tpool *p);

void tpool_push(void (*work)(void*), void *arg);

#endif
