#include <omp.h>
#include <stdio.h>

#include <cstdlib>

using namespace std;

/*
 * calculate fibonacci number
 */
int fib(int n) {
  if (n < 2)
    return n;
  else {
    int i, j;
#pragma omp task shared(i) firstprivate(n)
    i = fib(n - 1);

#pragma omp task shared(j) firstprivate(n)
    j = fib(n - 2);

#pragma omp taskwait
    return i + j;
  }
}

int main(int argc, char** argv) {
  int n = 0;
  int np = 0;

  if (argc != 3) {
    printf(" %s Number_of_threads Fibonacci_number \n", argv[0]);
    return -1;
  }

  np = atoi(argv[1]);
  if (np < 1) {
    printf("Error: Number_of_threads (%i) < 1 \n", np);
    return -1;
  }

  n = atoi(argv[2]);  //* 1024;
  if (n < 1) {
    printf("Error: Fibonacci_number (%d) < 1 \n", n);
    return -1;
  }

  double StartTime;
  double StopTime;

  omp_set_num_threads(np);

  StartTime = omp_get_wtime();
#pragma omp parallel shared(n)
  {
#pragma omp single
    printf("fib(%d) = %d\n", n, fib(n));
  }

  StopTime = omp_get_wtime();
  printf("Time to calculate fib %8.3fs\n", StopTime - StartTime);

  return 0;
}  // main()

