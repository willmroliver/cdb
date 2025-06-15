#ifndef INC_SCHED_SPOOL_H
#define INC_SCHED_SPOOL_H

#include "fiber.h"

/* @todo - try determine cache line size at build time */
#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

/* spools are the basic unit of synchronisation between fibers */
typedef struct __attribute__((packed)) {
	uint8_t state[CACHE_LINE_SIZE];
} spool_t;

void spool_init(spool_t *s);

/* --- ASM METHODS BEGIN --- */

extern int spool_try(spool_t *s, fiber_t *f);

extern int spool_wait(spool_t *s, fiber_t *f);

extern int spool_release(spool_t *s, fiber_t *f);

extern fiber_t *spool_bearer(spool_t *s);

extern uint64_t spool_inc(spool_t *s);

extern uint64_t spool_dec(spool_t *s);

/* --- ASM METHODS END --- */

#endif
