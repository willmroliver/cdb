#include "alg.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int test_min(const void *a, const void *b) {
  return *(int*)a < *(int*)b ? -1 : 1;
}

int main() {
  srand(time(NULL));
  int vals[1000] = {0};
  int passed = 1;

  for (int i = 0; i < 1000; ++i) {
    for (int j = 0; j <= i; ++j) {
      vals[j] = rand();
    }

    heap_sort(vals, i + 1, sizeof(int), test_min);


    for (int j = 0; j < i; ++j) {
      if (vals[j] > vals[j + 1]) {
        passed = 0;
        break;
      }
    }

    if (!passed) {
      break;
    }
  }

  if (passed) {
    printf("heap test passed\n");
  } else {
    printf("heap test failed\n");
  }
  
  return 0;
}
