#include <omp.h>
#include <stdio.h>

#include <cstdlib>

#include "vector_add.h"

using namespace std;

/*
 *  main
 */
int main(int argc, char** argv) {
  int np = 0;
  size_t size = 0;
  size_t nrpt = 0;

  if (argc != 4) {
    printf(" %s Number_of_threads Number_of_elements Number_of_repetitions\n",
           argv[0]);
    return -1;
  }

  np = atoi(argv[1]);
  if (np < 1) {
    printf("Error: Number_of_threads (%i) < 1 \n", np);
    return -1;
  }

#ifdef PERFORMANCE
  size = atoi(argv[2]) * 1024;
#else
  size = atoi(argv[2]);
#endif

  if (size < 1) {
    printf("Error: Number_of_elements (%ld) < 1 \n", size);
    return -1;
  }

  nrpt = atoi(argv[3]);
  if (size < 1) {
    printf("Error: Number_of_repetitions (%ld) < 1 \n", nrpt);
    return -1;
  }

#ifdef PERFORMANCE
  printf("Number of elements: %ldK\n", size);
  if (((3 * size) / (1 << 20)) == 0) {
    printf("Total memory      : %ldKB\n", 3 * size * sizeof(float) / 1024);
  } else {
    printf("Total memory      : %ldMB\n",
           3 * size * sizeof(float) / 1024 / 1024);
  }
#endif

  omp_set_num_threads(np);

  ExecuteVectorAdds(size, nrpt);

  return 0;
}

