#include "spindle.h"

#include <stdlib.h>

/* --- PRIVATE METHODS BEGIN --- */

struct spindle_arg {
	void *(*proc)(void*);
	void *arg;
	struct spindle *s;
	dl_node_t *fiber;
};

void *spindle_job(void *arg);

void spindle_job_done(struct spindle_arg *sa);

/* --- PRIVATE METHODS END --- */

void spindle_init(struct spindle *s, uint32_t size)
{
	uint32_t i;
	fiber_t *f;

	s->fibers = malloc(size * sizeof(fiber_t));
	s->idle = ring_alloc(size, sizeof(fiber_t*));
	s->running = dl_list_alloc();
	s->current = NULL;

	for (i = 0; i < size; ++i) {
		f = s->fibers + i;
		fiber_init(f, 0x1000);
		ring_push(s->idle, &f);
	}
}

void spindle_del(struct spindle *s)
{
	fiber_t *f;

	if (s == NULL)
		return;

	while (!ring_empty(s->idle)) {
		ring_pop(s->idle, &f);
		fiber_del(f);
	}

	dl_node_t *node = NULL;

	while (!dl_list_empty(s->running) && (node = dl_list_next(s->running, node))) {
		dl_list_remove(s->running, node);
		fiber_del(node->val);
	}

	ring_free(s->idle);
	dl_list_free(s->running);
}

int spindle_do(struct spindle *s, struct job j)
{
	fiber_t *f;
	dl_node_t *node;
	struct spindle_arg *sa = malloc(sizeof(struct spindle_arg));

	if (ring_empty(s->idle) || ring_pop(s->idle, &f))
		return -1; 

	if ((node = dl_list_insert(s->running, f)) == NULL)
		return -1;

	sa->proc = j.proc;
	sa->arg = j.arg;
	sa->s = s;
	sa->fiber = node;

	j.arg = sa;
	j.proc = spindle_job;

	fiber_do(f, j);
	return 0;
}

void spindle_loop(struct spindle *s)
{
	dl_node_t *next;

	while (!dl_list_empty(s->running)) {
		next = dl_list_next(s->running, s->current);
		/* if we've reached the end, start again */
		s->current = next ? next : dl_list_next(s->running, NULL);

		fiber_run(s->current->val);
	}
}

/* --- PRIVATE METHODS BEGIN --- */

void *spindle_job(void *arg)
{
	struct spindle_arg *sa = *(struct spindle_arg**)arg;
	*(void**)arg = sa->arg;
	sa->proc(arg);

	spindle_job_done(sa);

	return arg;
}

void spindle_job_done(struct spindle_arg *sa)
{
	ring_push(sa->s->idle, sa->fiber->val);
	dl_list_remove(sa->s->running, sa->fiber);
}

/* --- PRIVATE METHODS END --- */

