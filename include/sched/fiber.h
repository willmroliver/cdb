#ifndef INC_SCHED_FIBER_H
#define INC_SCHED_FIBER_H

#include "job.h"

#include <stdint.h>

struct __attribute__((packed)) fiber {
	uint64_t size;
	void *stack;
	void *rip;
	void *rsp;
	struct job job;
};

void fiber_init(struct fiber *f, uint32_t size16);

void fiber_del(struct fiber *f);

void fiber_do(struct fiber *f, struct job j);

void fiber_yield();

#endif
