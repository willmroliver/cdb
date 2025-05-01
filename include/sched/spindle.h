#ifndef INC_SCHED_SPINDLE_H
#define INC_SCHED_SPINDLE_H

#include "fiber.h"
#include "job.h"
#include "ds/ring.h"

struct __attribute__((packed)) spindle {
	fiber_t *active;
	ring_t *idle;
	ring_t *busy;
};

void spindle_init(struct spindle *s, uint32_t size);

void spindle_del(struct spindle *s);

int spindle_do(struct spindle *s, struct job j);

#endif
