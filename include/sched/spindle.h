#ifndef INC_SCHED_SPINDLE_H
#define INC_SCHED_SPINDLE_H

#include "fiber.h"
#include "job.h"
#include "ds/ring.h"
#include "ds/dl_list.h"

struct __attribute__((packed)) spindle {
	fiber_t *fibers;
	fiber_t *running;
	ring_t *idle;
	dl_list_t *busy;
};

void spindle_init(struct spindle *s, uint32_t size);

void spindle_del(struct spindle *s);

int spindle_do(struct spindle *s, struct job j);

#endif
