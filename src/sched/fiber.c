#include "sched/fiber.h"

#include <stdlib.h>

void fiber_init(struct fiber *f, uint32_t size16)
{
	f->size = 16 * size16;
	f->stack = malloc(16 * size16);
}

void fiber_del(struct fiber *f)
{
	if (f == NULL)
		return;

	if (f->stack != NULL)
		free(f->stack);

	free(f);
}

void fiber_do(struct fiber *f, struct job j)
{

}

void fiber_yield();

