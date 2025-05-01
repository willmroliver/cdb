#include "sched/fiber.h"

#include <stdlib.h>

void fiber_init(fiber_t *f, uint32_t size16)
{
	f->size = 16 * size16;
	f->stack = size16 ? malloc(16 * size16) : NULL;
	f->meta = 0;
	f->rip = 0;
	f->rsp = (char*)f->stack + f->size - 8;
}

void fiber_del(fiber_t *f)
{
	if (f == NULL)
		return;

	if (f->stack != NULL)
		free(f->stack);
}

void fiber_do(fiber_t *f, struct job j)
{
	f->job = j;
}

