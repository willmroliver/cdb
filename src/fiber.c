#include "fiber.h"

#include <stdlib.h>
#include <string.h>

void fiber_init(fiber_t *f, uint32_t size16)
{
	memset(f->meta, 0, sizeof(f->meta));
	f->size = 16 * size16;
	f->flags = 0;
	f->stack = size16 ? malloc(16 * size16) : NULL;
	f->rip = 0;
	f->rsp = (char*)f->stack + f->size - 8;
}

void fiber_del(fiber_t *f)
{
	if (f == NULL)
		return;
	if (f->stack != NULL)
		free(f->stack);
	f->flags = 0;
}

void fiber_do(fiber_t *f, struct job j)
{
	f->job = j;
	f->flags |= FIBER_READY;
}

uint64_t fiber_flags_test(fiber_t *f, uint64_t and)
{
	return f->flags & and;
}

uint64_t fiber_set_recurring(fiber_t *f, int recur)
{
	f->flags = (f->flags & ~FIBER_RECURRING) | (recur ? FIBER_RECURRING : 0);
	return f->flags;
}
