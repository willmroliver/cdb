#include "sched/spool.h"

#include <string.h>
#include <stdint.h>

void spool_init(spool_t *s)
{
	memset(s->state, 0, sizeof(s->state));
}

