#include "alg.h"
#include "util.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>

// --- UNIT TESTS ---
int sort_test(char*, size_t, int (*)(int*, size_t));
int heap_sort_test(int*, size_t);
int merge_sort_test(int*, size_t);

int search_test(char*, size_t, int (*)(int, int*, size_t, int*));
int b_search_test(int, int*, size_t, int*);

// --- UTILITY FUNCTIONS ---
int assert_cmp(void*, size_t, size_t, int(*)(const void*, const void*));
int min_cmp(const void*, const void*);
void rand_ints(int*, size_t, size_t);

int main() {
  int passed = 
    sort_test("heapsort", 1000, heap_sort_test) &&
    sort_test("mergesort", 1000, merge_sort_test) &&
    search_test("bsearch", 1000, b_search_test);

  printf(passed ? "alg tests passed\n" : "alg tests failed\n");

  return 0;
}

// --- UNIT TESTS ---

int sort_test(char* name, size_t n, int (*func)(int*, size_t)) {
  int result;
  struct timespec from;
  size_t ms;

  clock_gettime(CLOCK_REALTIME, &from);

  for (size_t i = 1; i < n; ++i) {
    int *vals = malloc(i * sizeof(int));
    rand_ints(vals, i, 10);

    result = func(vals, i);

    free(vals);

    if (result == 0) {
      return 0;
    }
  }

  ms = clock_timesince_nsec(CLOCK_REALTIME, &from)/1e6;
  printf("%zu ms - %s\n", ms, name);
  return 1;
}

int heap_sort_test(int *vals, size_t len) {
  heap_sort(vals, len, sizeof(int), min_cmp);

  if (!assert_cmp(vals, len, sizeof(int), min_cmp)) {
    printf("heap sort failed\n");
    return 0;
  }

  return 1;
}

int merge_sort_test(int *vals, size_t len) {
  merge_sort(vals, 0, len, sizeof(int), min_cmp);

  if (!assert_cmp(vals, len, sizeof(int), min_cmp)) {
    printf("merge sort failed\n");
    return 0;
  }

  return 1;
}

int search_test(char *name, size_t n, int (*func)(int, int*, size_t, int*)) {
  int result;
  struct timespec from;
  size_t ms;

  clock_gettime(CLOCK_REALTIME, &from);

  for (size_t i = 1; i < n; ++i) {
    int *vals = malloc(n * sizeof(int));
    rand_ints(vals, i, 2*n);
    qsort(vals, i, sizeof(int), min_cmp);
    int key = vals[rand() % i];
    int oob = 2*n + 1;

    result =
      func(key, vals, i, &key) &&
      func(oob, vals, i, NULL);

    free(vals);

    if (result == 0) {
      return 0;
    }
  }

  ms = clock_timesince_nsec(CLOCK_REALTIME, &from)/1e6;
  printf("%zu ms - %s\n", ms, name);
  return 1;
}

int b_search_test(int key, int *vals, size_t len, int *exp) {
  int *result = b_search(&key, vals, len, sizeof(int), min_cmp);

  if (
    (exp == NULL && result != NULL) ||
    (exp != NULL && (
    result == NULL ||
    *exp != *result
  ))) {
    printf("b search failed\nexpecting %d, got %d in [", exp == NULL ? -1 : *exp, result == NULL ? -1 : *result);
    for (size_t i = 0; i < len; ++i) printf(" %d", vals[i]);
    printf(" ]\n");
    return 0;
  }

  return 1;
}

// --- UTILITY FUNCTIONS ---

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
  if (*(int*)a < *(int*)b) {
    return -1;
  } else if (*(int*)a > *(int*)b) {
    return 1;
  } else {
    return 0;
  }
}

void rand_ints(int *nums, size_t n, size_t lim) {
  srand(time(NULL));

  for (size_t i = 0; i < n; ++i) {
    nums[i] = rand() % lim;
  }
}

