#include "sched/fiber.h"

#include <stdlib.h>

void fiber_init(struct fiber *f, uint32_t size16)
{
	f->size = 16 * size16;
	f->stack = size16 ? malloc(16 * size16) : NULL;
}

void fiber_del(struct fiber *f)
{
	if (f == NULL)
		return;

	if (f->stack != NULL)
		free(f->stack);

	free(f);
}

