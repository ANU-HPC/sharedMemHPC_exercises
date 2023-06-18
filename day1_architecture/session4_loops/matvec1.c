#include <math.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "myvec.h"

#define NVERSIONS 2

#define DEBUG 0
#define NEXPT 20

#define PAPI_COUNT_ARRAY_LEN 1

void prtmatrix(const char label[], double *matrix, int ncol, int nrow);
void elp_time(int *isec, int *iusec);
void matvecmult(int vers, int trans, int n, double a[], double b[], double x[]);
void ur_matvecmult(int vers, int trans, int n, double a[], double b[],
                   double x[]);

int main(int argc, char *argv[]) {
  int nsize;
  char name[21];
  int vers, trans, i;
  double *a, *b, *c, *c_kp;
  int isec0, iusec0, isec1, iusec1;
  int delta, delta_min, repeat, expt;
  double tmatvec, err;

  float mflops;
  long long values[PAPI_COUNT_ARRAY_LEN];
  int events[PAPI_COUNT_ARRAY_LEN] = {PAPI_DP_OPS};

  if (argc != 2) {
    printf(" %s Matrix_Dimension \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
  }

  a = (double *)malloc(nsize * nsize * sizeof(double));
  b = (double *)malloc(nsize * sizeof(double));
  c = (double *)malloc(nsize * sizeof(double));
  c_kp = (double *)malloc(nsize * sizeof(double));

  if (!(a && b && c && c_kp)) {
    printf(" Allocation failure reduce n %d \n", nsize);
    exit(-1);
  }
  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Matrix Vector Multiplication Benchmark\n");
  printf("Running on    :  %20s\n", name);
  printf("Size of Matrix:  %20d\n", nsize);
  printf("--------------------------------------------\n");

  // initialize our arrays
  for (i = 0; i < nsize * nsize; i++) a[i] = 1.0 / (double)(i + 1);
  for (i = 0; i < nsize; i++) b[i] = 1.0 / (double)(i + 1);

  if (DEBUG) {
    prtmatrix("Initial Amatrix ", a, nsize, nsize);
    prtmatrix("Initial bvector ", b, 1, nsize);
  }

  PAPI_library_init(PAPI_VER_CURRENT);

  printf("\n\n");
  printf(
      "Transpose  Version   NSIZE    REPEAT    Time(sec)        MFLOPS         "
      "Error\n");
  printf(
      "------------------------------------------------------------------------"
      "-----\n");

  for (trans = 0; trans < 2; trans++) {
    for (vers = 0; vers < NVERSIONS; vers++) {
      delta_min = 0;
      repeat = 1;
      /* increase "repeat" until each experiment is at least 500us */
      while (delta_min <= 500) {
        for (i = 0; i < nsize; i++) c[i] = 0.0;
        repeat *= 2;
        for (expt = 0; expt < NEXPT; expt++) {
          /* Time repeated Matrix vector Product*/

          if (PAPI_start_counters(events, PAPI_COUNT_ARRAY_LEN) != PAPI_OK)
            perror("Error in PAPI_start_counters");
          elp_time(&isec0, &iusec0);

          for (i = 0; i < repeat; i++) matvecmult(vers, trans, nsize, a, b, c);

          elp_time(&isec1, &iusec1);
          if (PAPI_stop_counters(values, PAPI_COUNT_ARRAY_LEN) != PAPI_OK)
            perror("Error in PAPI_stop_counters");

          delta = (isec1 - isec0) * 1000000 + iusec1 - iusec0;
          if (expt == 1) delta_min = delta;
          delta_min = (delta_min < delta) ? delta_min : delta;
          mflops = values[0] / delta;
        }
      }

      /* Some basic error checking */
      for (i = 0; i < nsize; i++) c[i] /= (float)repeat;

      err = 0.0;

      if (vers != 0) {
        for (i = 0; i < nsize; i++)
          err = (err > fabs(c[i] - c_kp[i])) ? err : fabs(c[i] - c_kp[i]);
      }

      for (i = 0; i < nsize; i++) c_kp[i] = c[i];

      if (err > 1.0e-6) printf("PROBABLE ERROR %e\n", err);

      tmatvec = 1.0e-6 * (double)delta_min / (double)repeat;
      printf("%-11s%-9s%6i%9i%14.6f%14.6f%14.2e\n",
             trans ? "Transpose" : "Normal", vers ? "Daxpy" : "Dot", nsize,
             repeat, tmatvec, mflops, err);

      if (DEBUG) {
        prtmatrix("Final cvector ", c, 1, nsize);
      }
    }
  }
  printf(
      "------------------------------------------------------------------------"
      "-----\n");

}  // main()

void matvecmult(int vers, int trans, int n, double a[], double b[],
                double c[]) {
  int i;
  switch (vers) {
    case 0:
      // dot product based matrix vector product
      if (trans != 0) {
        for (i = 0; i < n; i++) mydot(&c[i], n, &a[i], n, b, 1);
      } else {
        for (i = 0; i < n; i++) mydot(&c[i], n, &a[i * n], 1, b, 1);
      }
      break;
    case 1:
      // daxpy based matrix vector product
      if (trans != 0) {
        for (i = 0; i < n; i++) mydaxpy(n, b[i], c, 1, &a[i * n], 1);
      } else {
        for (i = 0; i < n; i++) mydaxpy(n, b[i], c, 1, &a[i], n);
      }
      break;
    default:
      printf("incorrect function\n");
      abort();
  }
}  // matvecmult()

void prtmatrix(const char label[], double *matrix, int ncol, int nrow) {
  int icount, row, col;
  printf("\n Prtmatrix : %-40s \n", label);
  icount = 0;
  for (row = 0; row < nrow; row++) {
    printf("Row%4d ", row);
    for (col = 0; col < ncol; col++) {
      printf("%12.3e", matrix[icount]);
      icount++;
      if (col % 6 == 5) printf("\n        ");
    }
    printf("\n");
  }
}  // prtmatrix()

void elp_time(int *isec, int *iusec) {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  *isec = tp.tv_sec;
  *iusec = tp.tv_usec;
}  // elp_time()
