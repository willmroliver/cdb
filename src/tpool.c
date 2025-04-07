#include "tpool.h"

#include <unistd.h>
#include <stdlib.h>

void tpool_init(struct tpool *p) {
  p->size = sysconf(_SC_NPROCESSORS_CONF);
  p->pool = malloc(p->size * sizeof(pthread_t));
}

