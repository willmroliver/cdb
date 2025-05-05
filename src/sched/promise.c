#include "sched/promise.h"
#include <string.h>
#include <stdlib.h>

void promise_init(struct promise *p, size_t size, size_t count)
{
	size_t i;

	spool_init((spool_t*)p);
	for (i = 0; i < count; ++i)
		spool_inc(&p->spool);

	p->size = size;
	p->count = count;
	p->value = malloc(size * count); 
}

void promise_del(struct promise *p)
{
	if (p == NULL)
		return;
	free(p->value);
}

void promise_resolve(struct promise *p, void *value)
{
	size_t i = spool_dec((spool_t*)p);
	memcpy(p->value + p->size * (p->count - (i + 1)), value, p->size);
}

void *promise_wait(struct promise *p, fiber_t* f)
{
	spool_wait((spool_t*)p, f);
	return p->value;	
}

