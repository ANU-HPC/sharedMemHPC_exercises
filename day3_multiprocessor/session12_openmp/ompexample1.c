#include <omp.h>
#include <stdio.h>

int main(void) {
  int i = 1, j = 2, k;

  printf(" Initial value of i %i and j %i \n", i, j);

#pragma omp parallel for default(shared) private(i)
  for (k = 0; k < 4; k++) {
    printf("Initial value in parallel of i %i and j %i\n", i, j);
    i = i + 10;
    j = j + 10;
    printf("Final value in parallel of i %i and j %i\n", i, j);
  }

  printf(" Final value of i %i and j %i \n", i, j);
  return 0;
}
