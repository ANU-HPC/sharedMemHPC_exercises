#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  int np, t_id, num_threads, max_threads;

  if (argc != 2) {
    printf(" %s Number_of_threads \n", argv[0]);
    return -1;
  } else {
    np = atoi(argv[1]);
    if (np < 1) {
      printf("Error: Number_of_threads (%i) < 1 \n", np);
      return -1;
    }
  }

  omp_set_num_threads(np);

  num_threads = omp_get_num_threads();
  max_threads = omp_get_max_threads();
  printf("Before Parallel: num_threads=%i max_threads %i\n", num_threads,
         max_threads);
#pragma omp parallel default(none) private(num_threads, t_id)
  {
    num_threads = omp_get_num_threads();
    t_id = omp_get_thread_num();
    printf("In Parallel: num_threads=%i t_id=%i \n", num_threads, t_id);
  }
  num_threads = omp_get_num_threads();
  printf("After Parallel: num_threads=%i \n", num_threads);

  return 0;
}  // main()
