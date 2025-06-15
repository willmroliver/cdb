#ifndef INC_SCHED_SPINDLE_H
#define INC_SCHED_SPINDLE_H

#include "fiber.h"
#include "job.h"
#include "ds/ring.h"
#include "ds/dl_list.h"
#include <pthread.h>

struct __attribute__((packed)) spindle {
	fiber_t *fibers;
	ring_t *idle;
	dl_list_t *running;
	dl_node_t *current;
};

void spindle_init(struct spindle *s, uint32_t size);

void spindle_del(struct spindle *s);

int spindle_do(struct spindle *s, struct job j);

void spindle_loop(struct spindle *s);

/* assimilates the current stack and execution context, */
/* returning a spindle which owns the current call stack */
/* as an asynchronous job */
struct spindle *spindle_hijack();

void spindle_release(struct spindle *s);

#endif

