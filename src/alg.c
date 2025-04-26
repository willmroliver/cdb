#include "alg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void print_heap(char *cbase, size_t nel, size_t width)
{
	size_t i, base = 1;

	for (i = 0; i < nel; ++i) {
		if (i + 1 == base) {
			printf("\n");
			base *= 2;
		}

		printf("%d ", *(int*)(cbase + (i * width)));
	}

	printf("\n");
}

void make_heap(void *base, size_t nel, size_t width, size_t i, int (*compar)(const void *, const void *))
{
	char *cbase = (char*)base;
	void *hold;

	size_t left = 2 * i + 1, right = 2 * i + 2;
	size_t max = i;

	if (left < nel && compar(cbase + width * max, cbase + width * left) < 0)
		max = left;

	if (right < nel && compar(cbase + width * max, cbase + width * right) < 0)
		max = right;

	if (max == i)
		return;

	hold = malloc(width);

	memcpy(hold, cbase + (i * width), width);
	memcpy(cbase + (i * width), cbase + (max * width), width);
	memcpy(cbase + (max * width), hold, width);

	make_heap(cbase, nel, width, max, compar);

	free(hold);
}

void heap_sort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *))
{
	char *cbase = (char*)base;
	void *hold;

	size_t i, from = nel > 1 ? nel/2 - 1 : 0;

	for (i = from; ; --i) {
		make_heap(cbase, nel, width, i, compar);
		if (i == 0)
			break;
	}

	hold = malloc(width);
	from = nel > 0 ? nel - 1 : 0;

	for (i = from; ; --i) {
		memcpy(hold, cbase, width);
		memcpy(cbase, cbase + (i * width), width);
		memcpy(cbase + (i * width), hold, width);

		make_heap(cbase, i, width, 0, compar);

		if (i == 0)
			break;
	}

	free(hold);
}

void merge_sort(void *base, size_t from, size_t to, size_t width, int(*compar)(const void*, const void*))
{
	char *cbase = (char*)base;
	void *srtd;
	size_t len, mid, i, j, k;

	if (from + 1 == to)
		return;

	len = to - from;
	mid = from + len/2;

	merge_sort(base, from, mid, width, compar);
	merge_sort(base, mid, to, width, compar);

	i = from;
	j = mid;
	srtd = malloc(width * len);

	for (k = 0; k < len; ++k) {
		if (i < mid && ((j == to) || compar(cbase + i * width, cbase + j * width) < 0)) {
			memcpy(srtd + k * width, cbase + i * width, width);
			++i;
		} else {
			memcpy(srtd + k * width, cbase + j * width, width);
			++j;
		}
	}

	memcpy(cbase + from * width, srtd, width * len);

	free(srtd);
}

void *b_search(const void *key, const void *base, size_t nel, size_t width, int (*compar)(const void*, const void*))
{
	size_t left = 0, right = nel, mid;
	char *cbase = (char*)base;
	int cmp;

	while (left < right) {
		mid = left + (right - left)/2;

		cmp = compar(key, cbase + mid * width);

		if (cmp == 0)
			return cbase + mid * width;

		if (cmp < 0)
			right = mid;
		else
			left = mid + 1;
	}

	return NULL;
}
