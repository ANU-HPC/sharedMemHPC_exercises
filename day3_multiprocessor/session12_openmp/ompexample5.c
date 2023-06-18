#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  int np;
  int nele, i;
  double xl, fxl, xh, fxh, xw, area;

  if (argc != 3) {
    printf(" %s Number_of_threads Number_of_segments\n", argv[0]);
    return -1;
  } else {
    np = atoi(argv[1]);
    nele = atoi(argv[2]);
    if (np < 1 || nele < 1) {
      printf("Error: Number_of_threads  (%i) < 1 \n", np);
      printf("Error: Number_of_segments (%i) < 1 \n", nele);
      return -1;
    }
  }

  xl = 0.0;
  xw = 1.0 / nele;
  area = 0.0;
  for (i = 0; i < nele; i++) {
    xh = xl + xw;
    fxl = 1.0 / (1.0 + xl * xl);
    fxh = 1.0 / (1.0 + xh * xh);
    area += 0.5 * (fxl + fxh) * xw;
    xl = xh;
  }

  printf("Elements %i, Pi %.16f \n", nele, area * 4.0);

  return 0;
}  // main()
