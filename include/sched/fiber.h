#ifndef INC_SCHED_FIBER_H
#define INC_SCHED_FIBER_H

#include "job.h"

#include <stdint.h>

typedef struct __attribute__((packed)) fiber {
	uint64_t size;
	void *stack;
	void *meta;
	void *rip;
	void *rsp;
	struct job job;
} fiber_t;

void fiber_init(fiber_t *f, uint32_t size16);

void fiber_del(fiber_t *f);

void fiber_do(fiber_t *f, struct job j);

/* --- ASM METHODS BEGIN --- */

extern void fiber_run(fiber_t *f);

extern void fiber_yield(fiber_t *f);

extern fiber_t *fiber_self(void *arg);

/* --- ASM METHODS END --- */

#endif
