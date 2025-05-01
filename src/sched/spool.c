#include "sched/spool.h"

#include <stdint.h>

struct __attribute__((packed)) spool {
	uint8_t state[CACHE_LINE_SIZE];
};

