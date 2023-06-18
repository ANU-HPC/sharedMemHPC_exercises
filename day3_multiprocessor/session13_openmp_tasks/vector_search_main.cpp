#include <omp.h>
#include <stdio.h>

#include <cstdlib>

#include "vector_add.h"
#include "vector_search.h"

using namespace std;

int main(int argc, char** argv) {
  int np = 0;
  long size = 0;
  float key = 0;

  if (argc != 4) {
    printf(" %s Number_of_threads Number_of_elements Key\n", argv[0]);
    return -1;
  }

  np = atoi(argv[1]);
  if (np < 1) {
    printf("Error: Number_of_threads (%i) < 1 \n", np);
    return -1;
  }

  size = atoi(argv[2]);
  if (size < 1) {
    printf("Error: Number_of_elements (%ld) < 1 \n", size);
    return -1;
  }

  key = atoi(argv[3]);

  omp_set_num_threads(np);

  ExecuteVectorSearches(size, key);

  return 0;
}

