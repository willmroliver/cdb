#include "ds/ring.h"

#include <string.h>

struct ring {
  uint32_t nel;
  uint32_t width;
  uint64_t start;
  uint64_t end;
  void *data;
};

ring_t *ring_alloc(uint32_t nel, uint32_t width) 
{
  ring_t *t;

  if ((nel ^ (nel - 1)) != 2 * nel - 1)
    return NULL;

  t = malloc(sizeof(ring_t));
  t->nel = nel;
  t->width = width;
  t->start = 0;
  t->end = 0;
  t->data = malloc(nel * width);

  return t;
}

void ring_free(ring_t *t) 
{
  free(t->data);
  free(t);
}

int ring_push(ring_t *t, void *val)
{
  if (ring_full(t))
    return 1;

  memcpy(t->data + t->width * (t->end & t->nel-1), val, t->width);
  t->end = (t->end + 1) & (2*t->nel - 1);
  return 0;
}

int ring_pop(ring_t *t, void *val)
{
  if (ring_empty(t))
    return 1;

  memcpy(val, t->data + t->width * (t->start & t->nel-1), t->width);
  t->start = (t->start + 1) & (2*t->nel - 1);
  return 0;
}

int ring_full(ring_t *t) 
{
  return t->end - t->start == t->nel;
}

int ring_empty(ring_t *t) 
{
  return t->start == t->end;
}


