#include <stdio.h>

#include <cstdlib>

#include "simple_papi.h"
#include "vector_base.h"
#include "vector_dot.h"

using namespace std;

int main(int argc, char** argv) {
  size_t size = 0;
  size_t nrpt = 0;

  if (argc != 3) {
    printf(" %s Number_of_elements Number_of_repetitions\n", argv[0]);
    return -1;
  }

#ifdef PERFORMANCE
  size = atoi(argv[1]) * 1024;
#else
  size = atoi(argv[1]);
#endif

  if (size < 1) {
    printf("Error: Number_of_elements (%ld) < 1 \n", size);
    return -1;
  }

  nrpt = atoi(argv[2]);
  if (size < 1) {
    printf("Error: Number_of_repetitions (%ld) < 1 \n", nrpt);
    return -1;
  }

#ifdef PERFORMANCE
  printf("Number of elements: %ld\n", size);
  if (((2 * size) / (1 << 20)) == 0) {
    printf("Total memory      : %ldKB\n", 2 * size * sizeof(float) / 1024);
  } else {
    printf("Total memory      : %ldMB\n",
           2 * size * sizeof(float) / 1024 / 1024);
  }
#endif

  InitPapi();

  ExecuteVectorDots(size, nrpt);

  return 0;
}

