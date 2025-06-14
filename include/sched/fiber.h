#ifndef INC_SCHED_FIBER_H
#define INC_SCHED_FIBER_H

#include "job.h"

#include <stdint.h>

/* fiber state flags */
#define FIBER_READY	  0x01	/* fiber has been assigned work	    */ 
#define FIBER_RUNNING	  0x02	/* fiber has started a job	    */
#define FIBER_YIELDED	  0x04	/* fiber has yielded mid-job	    */
#define FIBER_DONE	  0x08	/* fiber finished job execution	    */
#define FIBER_RECURRING	  0x10	/* fiber restarts job on completion */
#define FIBER_HIJACKER	  0x20  /* fiber is 'hijacker' of main proc */
#define FIBER_PARENT	  0x40	/* fiber is 'parent' to 'hijacker'  */

/* fiber_t represents a unit of resource to execute work co-operatively */
/**/
/* most of these fields should not be directly manipulated in C */
typedef struct __attribute__((packed)) {
	uint64_t meta[4];
	uint64_t size;
	uint64_t flags;
	void *stack;
	void *rip;
	void *rsp;
	struct job job;
} fiber_t;

void fiber_init(fiber_t *f, uint32_t size16);

void fiber_del(fiber_t *f);

void fiber_do(fiber_t *f, struct job j);

uint64_t fiber_flags_test(fiber_t *f, uint64_t and);

uint64_t fiber_set_recurring(fiber_t *f, int recur);

/* --- ASM METHODS BEGIN --- */

extern void fiber_run(fiber_t *f);

extern void fiber_yield(fiber_t *f);

extern fiber_t *fiber_self(void *arg);

extern fiber_t *fiber_hijack(fiber_t *f);

/* --- ASM METHODS END --- */

#endif
