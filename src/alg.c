#include "alg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void print_heap(char *cbase, size_t nel, size_t width) {
  size_t base = 1;

  for (size_t i = 0; i < nel; ++i) {
    if (i + 1 == base) {
      printf("\n");
      base *= 2;
    }

    printf("%d ", *(int*)(cbase + (i * width)));
  }

  printf("\n");
}

void make_heap(void *base, size_t nel, size_t width, size_t i, int (*compar)(const void *, const void *)) {
  char *cbase = (char*)base;

  size_t left = 2 * i + 1, right = 2 * i + 2;
  size_t max = i;

  if (left < nel && compar(cbase + width * max, cbase + width * left) < 0) {
    max = left;
  }
  
  if (right < nel && compar(cbase + width * max, cbase + width * right) < 0) {
    max = right;
  }

  if (max == i) {
    return;
  }

  void *hold = malloc(width);

  memcpy(hold, cbase + (i * width), width);
  memcpy(cbase + (i * width), cbase + (max * width), width);
  memcpy(cbase + (max * width), hold, width);

  make_heap(cbase, nel, width, max, compar);

  free(hold);
}

void heap_sort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *)) {
  char *cbase = (char*)base;
  
  size_t from = nel/2;
  if (from > 0) {
    --from;
  }

  for (size_t i = from; i >= 0; --i) {
    make_heap(cbase, nel, width, i, compar);
    if (i == 0) {
      break;
    }
  }

  void *hold = malloc(width);

  from = nel;
  if (from > 0) {
    --from;
  }

  for (size_t i = from; i >= 0; --i) {
    memcpy(hold, cbase, width);
    memcpy(cbase, cbase + (i * width), width);
    memcpy(cbase + (i * width), hold, width);
    
    make_heap(cbase, i, width, 0, compar);

    if (i == 0) {
      break;
    }
  }
};

