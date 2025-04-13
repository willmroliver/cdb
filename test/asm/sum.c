#include <stdio.h>

extern int sum(int a, int b);

int main()
{
  int result = sum(3, 4);
  int passed = result == 7;

  printf(passed ? "asm sum tests passed\n" : "asm sum tests failed: got %d\n", result);
  return 0;
}

