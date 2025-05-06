#include "sched/spindle.h"

#include <stdlib.h>

void spindle_init(struct spindle *s, uint32_t size)
{
	uint32_t i;
	fiber_t *f;

	s->fibers = malloc(size * sizeof(fiber_t));
	s->idle = ring_alloc(size, sizeof(fiber_t*));
	s->busy = dl_list_alloc();

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

	while (!dl_list_empty(s->busy) && (node = dl_list_next(s->busy, node))) {
		dl_list_remove(s->busy, node);
		fiber_del(node->val);
	}

	ring_free(s->idle);
	dl_list_free(s->busy);
}

int spindle_do(struct spindle *s, struct job j)
{
	fiber_t *f;
	dl_node_t *node;

	if (ring_empty(s->idle) || ring_pop(s->idle, &f))
		return -1;

	fiber_do(f, j);

	if ((node = dl_list_insert(s->busy, f)) == NULL)
		return -1;

	return 0;
}

