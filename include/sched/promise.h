#ifndef INC_SCHED_PROMISE_H
#define INC_SCHED_PROMISE_H

#include "spool.h"

#include <stddef.h>

struct promise {
	spool_t spool;
	size_t size;
	size_t count;
	void *value;
};

void promise_init(struct promise *p, size_t size, size_t plen);

void promise_del(struct promise *p);

void promise_resolve(struct promise *p, void *value);

void *promise_wait(struct promise *p, fiber_t *f);

#endif

