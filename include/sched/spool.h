#ifndef INC_SCHED_FUTEX_H
#define INC_SCHED_FUTEX_H

#include "fiber.h"

/* @todo - try determine cache line size at build time */
#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

/* spools are the basic unit of synchronisation between fibers */
typedef struct spool spool_t;

void spool_init(spool_t *s);

void spool_del(spool_t *s);

int spool_try(spool_t *s, fiber_t *f);

int spool_wait(spool_t *s, fiber_t *f);

int spool_yield(spool_t *s, fiber_t *f);

int spool_inc(spool_t *s);

int spool_dec(spool_t *s);

int spool_wake_one(spool_t *s);

int spool_wake_all(spool_t *s);

#endif
