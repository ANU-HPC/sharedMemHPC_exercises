#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void initval(int iset);
int nxtval();

int icount;

int main(int argc, char* argv[]) {
  int np;
  int mytask, maxtask, iam, sum, tsum, error;

  if (argc != 3) {
    printf(" %s Number_of_threads Number_of_tasks \n", argv[0]);
    return -1;
  } else {
    np = atoi(argv[1]);
    maxtask = atoi(argv[2]);
    if (np < 1 || maxtask < 1 || maxtask > 5000) {
      printf("Error: Number_of_threads  (%i) < 1 \n", np);
      printf("Error: Number_of_tasks 1 <= (%i) <= 5000 \n", maxtask);
      return -1;
    }
  }

  omp_set_num_threads(np);
  initval(0);
  sum = 0;

#pragma omp parallel default(none) shared(maxtask) private(mytask,iam) \
 reduction(+:sum)
  {
    mytask = nxtval();
    iam = omp_get_thread_num();
    while (mytask < maxtask) {
      // add up the task ids, should be 0+1+2+3....+maxtask-1
      sum += mytask;
      mytask = nxtval();
    }
  }

  /*We know what sum of all task ids should be*/
  tsum = maxtask * (maxtask - 1) / 2;
  error = sum - tsum;
  printf("sum %i tsum %i ERROR %i \n", sum, tsum, error);
  if (error != 0) printf("Something went wrong!\n");

  return 0;
}  // main()

void initval(int iset) {
  // initialize counter
  icount = iset - 1;
  return;
}

int nxtval() {
  // increment counter
  icount++;
  return icount;
}
