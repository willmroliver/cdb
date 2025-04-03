#ifndef INC_ALG_H
#define INC_ALG_H

#include <stddef.h>

/*could find these in the std library, but that's no fun*/

void make_heap(void *base, size_t nel, size_t width, size_t i, int (*compar)(const void *, const void *));

void heap_sort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));

void merge_sort(void *base, size_t from, size_t to, size_t width, int (*compar)(const void *, const void*));

void *b_search(const void *key, const void *base, size_t nel, size_t width, int (*compar)(const void*, const void*));

#endif

