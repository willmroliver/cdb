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

/* --- ASM METHODS BEGIN --- */

extern void fiber_run(struct fiber *f);

extern void fiber_yield(void *arg);

/* --- ASM METHODS END --- */

#endif
