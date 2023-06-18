#include <omp.h>
#include <stdio.h>

int main(void) {
  int tnumber;
  int i = 10, j = 10, k = 10;

  printf("Before parallel region: i=%i, j=%i, k=%i\n", i, j, k);

#pragma omp parallel default(none) private(tnumber) reduction(+:i)	\
  reduction(*:j) reduction(^:k)
  {
    tnumber = omp_get_thread_num() + 1;
    i = tnumber;
    j = tnumber;
    k = tnumber;
    printf("Thread %i: i=%i, j=%i, k=%i\n", tnumber, i, j, k);
  }

  printf("After parallel region: i=%5i, j=%5i, k=%5i\n", i, j, k);
  return 0;
}
