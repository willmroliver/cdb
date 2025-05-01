#include "sched/spindle.h"

void spindle_init(struct spindle *s, uint32_t size)
{
	uint32_t i;
	fiber_t f;

	s->idle = ring_alloc(size, sizeof(fiber_t));
	s->busy = ring_alloc(size, sizeof(fiber_t));

	for (i = 0; i < size; ++i) {
		fiber_init(&f, 0x1000);
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
	while (!ring_empty(s->busy)) {
		ring_pop(s->busy, &f);
		fiber_del(f);
	}

	ring_free(s->idle);
	ring_free(s->busy);
}

int spindle_do(struct spindle *s, struct job j)
{
	fiber_t *f;

	if (ring_empty(s->idle) || ring_pop(s->idle, &f))
		return -1;

	fiber_do(f, j);

	if (ring_push(s->busy, &f))
		return -1;

	return 0;
}

