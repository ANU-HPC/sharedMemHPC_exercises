#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void prtvec(const char label[], double* vec, int len);
void vadd(int n, double* a, double* b);

int main(int argc, char* argv[]) {
  double* a;
  int nsize, i;
  char name[21];
  if (argc != 2) {
    printf(" %s Vector_Dimension \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
    if (nsize > 500) {
      printf("Are you sure you want nsize = %d\n", nsize);
      return -1;
    }
  }
  a = (double*)malloc(nsize * sizeof(double));
  if (!a) {
    printf(" Allocation failure reduce n %d \n", nsize);
    exit(-1);
  }
  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Program to use vadd\n");
  printf("Running on   :  %20s\n", name);
  printf("Size of Array:  %20d\n", nsize);
  printf("--------------------------------------------\n");

  for (i = 0; i < nsize; i++) {
    a[i] = 1.0;
  }
  prtvec("inital a", a, nsize);
  vadd(nsize - 1, &a[1], &a[0]);
  prtvec("final a", a, nsize);
}  // main()

void prtvec(const char label[], double* vec, int len) {
  int i;
  printf("\n prtvec: %-40s \n", label);
  for (i = 0; i < len; i++) {
    printf("%12.3e", vec[i]);
    if (i % 6 == 5) printf("\n");
  }
  printf("\n");
}
