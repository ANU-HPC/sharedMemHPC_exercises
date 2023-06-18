#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  int np;
  int nele, sum, i;

  if (argc != 3) {
    printf(" %s Number_of_threads Number_of_elements\n", argv[0]);
    return -1;
  } else {
    np = atoi(argv[1]);
    nele = atoi(argv[2]);
    if (np < 1 || nele < 1) {
      printf("Error: Number_of_threads  (%i) < 1 \n", np);
      printf("Error: Number_of_elements (%i) < 1 \n", nele);
      return -1;
    }
  }
  omp_set_num_threads(np);
  sum = 0;
  i = 0;
  do {
    i++;
    sum += i;
  } while (i < nele);
  printf("Sum from 1 to %i = %i \n", nele, sum);

  return 0;
}  // main()
