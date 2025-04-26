#ifndef INC_RING_BUF_H
#define INC_RING_BUF_H

#include <stddef.h>
#include <stdint.h>

typedef struct ring ring_t;

ring_t *ring_alloc(uint32_t nel, uint32_t width);

void ring_free(ring_t *t);

int ring_push(ring_t *t, void *val);

int ring_pop(ring_t *t, void *val);

int ring_full(ring_t *t);

int ring_empty(ring_t *t);

uint32_t ring_size(ring_t *t);

#endif
