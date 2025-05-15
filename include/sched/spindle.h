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
/* returning a spindle which owns the current process */
/* as an asynchronous job */
struct spindle *spindle_hijack();

/* notes to self - as our hijack routine returns the spindle, */
/* we can place it at the top of the stack of main() or any async-able */
/* routine before anything else is placed on the stack.  */
/**/
/* that allows us to pinch the return address and stack without  */
/* preventing library users from adding to the stack their own data. */

#endif
