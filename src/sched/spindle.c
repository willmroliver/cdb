#include "sched/spindle.h"

/* --- ASM METHODS BEGIN --- */

extern void spindle_hijack();

/* --- ASM METHODS END --- */

void spindle_init(struct spindle *s, uint32_t size)
{
	uint32_t i;
	struct fiber f;

	s->idle = ring_alloc(size, sizeof(struct fiber));
	s->busy = ring_alloc(size, sizeof(struct fiber));

	for (i = 0; i < size; ++i) {
		fiber_init(&f, 0x1000);
		ring_push(s->idle, &f);
	}

	spindle_hijack();
}

void spindle_del(struct spindle *s)
{
	struct fiber f;

	if (s == NULL)
		return;

	while (!ring_empty(s->idle)) {
		ring_pop(s->idle, &f);
		fiber_del(&f);
	}
	while (!ring_empty(s->busy)) {
		ring_pop(s->busy, &f);
		fiber_del(&f);
	}

	ring_free(s->idle);
	ring_free(s->busy);
}

int spindle_do(struct spindle *s, struct job j)
{
	struct fiber f;

	if (ring_empty(s->idle) || ring_pop(s->idle, &f))
		return -1;

	f.job = j;

	if (ring_push(s->busy, &f))
		return -1;

	return 0;
}

