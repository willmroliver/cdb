#include "alg.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int sort_test(char*, size_t, int (*)(size_t));
int heap_sort_test(size_t);
int merge_sort_test(size_t);

int assert_cmp(void*, size_t, size_t, int(*)(const void*, const void*));
int min_cmp(const void*, const void*);
int *rand_ints(size_t);

int main() {
  int passed = 
    sort_test("heapsort", 1000, heap_sort_test) &&
    sort_test("mergesort", 1000, merge_sort_test);

  if (passed) {
    printf("sort tests passed\n");
  } else {
    printf("sort tests failed\n");
  }

  return 0;
}

int sort_test(char* name, size_t n, int (*func)(size_t)) {
  struct timespec from, to;
  clock_gettime(CLOCK_REALTIME, &from);

  for (size_t i = 1; i < n; ++i) {
    if (func(i) == 0) {
      return 0;
    }
  }

  clock_gettime(CLOCK_REALTIME, &to);
  to.tv_sec -= from.tv_sec;
  to.tv_nsec -= from.tv_nsec;

  if (to.tv_nsec < 0) {
    to.tv_sec -= 1;
    to.tv_nsec += 1e9;
  }

  size_t ms = 1e3*to.tv_sec + to.tv_nsec/1e6;
  printf("%zu ms - %s\n", ms, name);
  return 1;
}

int heap_sort_test(size_t len) {
  int *vals = rand_ints(len);
  int passed;

  heap_sort(vals, len, sizeof(int), min_cmp);
  passed = assert_cmp(vals, len, sizeof(int), min_cmp);

  free(vals);

  if (!passed) {
    printf("heap sort failed\n");
  }
  
  return passed;
}

int merge_sort_test(size_t len) {
  int *vals = rand_ints(len);
  int passed;

  merge_sort(vals, 0, len, sizeof(int), min_cmp);
  passed = assert_cmp(vals, len, sizeof(int), min_cmp);

  free(vals);
  
  if (!passed) {
    printf("heap sort failed\n");
  }
  
  return passed;
}

int assert_cmp(void *base, size_t nels, size_t width, int(*cmp)(const void*, const void*)) {
  if (nels < 2) {
    return 1;
  }

  for (size_t i = 0; i < nels-1; ++i) {
    if (cmp(base + i * width, base + (i + 1) * width) == 1) {
      return 0;
    }
  }

  return 1;
}

int min_cmp(const void *a, const void *b) {
  return *(int*)a <= *(int*)b ? -1 : 1;
}

int *rand_ints(size_t n) {
  srand(time(NULL));
  int *nums = malloc(n * sizeof(int));

  for (size_t i = 0; i < n; ++i) {
    nums[i] = rand() % 10;
  }

  return nums;
}

