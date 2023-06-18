#include <math.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#ifdef CBLAS_AVAILABLE
#include <cblas.h>
#endif

#define NVERSIONS 2
#define DEBUG 0
#define NEXPT 20

#define NUM_EVENTS 2

void prtmatrix(const char label[], double *matrix, int ncol, int nrow);
void elp_time(int *isec, int *iusec);
void matmuldot(int n, double a[], double b[], double c[]);
void matmuldax(int n, double a[], double b[], double c[]);
void matmulout(int n, double a[], double b[], double c[]);
void compute_matmul(int version, int nsize, int repeat, double *a, double *b,
                    double *c, double *c_kp);
double check_err(int n, double c[], double c_kp[]);

int main(int argc, char *argv[]) {
  int nsize;
  char name[21];
  int i, j;
  double *a, *b, *c, *c_kp;
  int repeat;

  if (argc != 2) {
    printf(" %s Matrix_Dimension \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
  }

  a = (double *)malloc(nsize * nsize * sizeof(double));
  b = (double *)malloc(nsize * nsize * sizeof(double));
  c = (double *)malloc(nsize * nsize * sizeof(double));
  c_kp = (double *)malloc(nsize * nsize * sizeof(double));

  if (!(a && b && c && c_kp)) {
    printf(" Allocation failure reduce n %d \n", nsize);
    exit(-1);
  }
  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Matrix Matrix Multiplication Benchmark\n");
  printf("Running on    :  %20s\n", name);
  printf("Size of Matrix:  %20d\n", nsize);
  printf("--------------------------------------------\n");

  // initialize arrays - make symmetric
  for (i = 0; i < nsize; i++) {
    for (j = 0; j < i; j++) {
      a[i * nsize + j] = 1.0 / (double)(i * nsize + j + 1);
      b[i * nsize + j] = 1.0 / (double)(i * nsize + j + 1);
      a[j * nsize + i] = a[i * nsize + j];
      b[j * nsize + i] = b[i * nsize + j];
    }
  }

  if (DEBUG) {
    prtmatrix("Initial Amatrix ", a, nsize, nsize);
    prtmatrix("Initial Bmatrix ", b, nsize, nsize);
  }
  // Set repeats so total time around 1s on 1Gflops machine
  repeat = (1000 / nsize) * (1000 / nsize) * (1000 / nsize) + 1;

  printf("\n\n");
  printf(
      "type      nsize    repeat       time(s)        MFLOP/s          "
      "Error\n");
  printf(
      "-----------------------------------------------------------------------"
      "\n");

#ifdef CBLAS_AVAILABLE
  /*
     First call should be to dgemm library, which is kept for comparison
  */
  float alpha = 1.0 * repeat, beta = 0.0;
  cblas_dgemm(CblasRowMajor, CblasTrans, CblasTrans, nsize, nsize, nsize, alpha,
              a, /* lda = */ nsize, b, /* ldb = */ nsize, beta, c_kp,
              /* ldc = */ nsize);
#else
  for (i = 0; i < nsize * nsize; i++) c_kp[i] = 0.0;
  matmuldot(nsize, a, b, c_kp);
  for (i = 0; i < nsize * nsize; i++) c_kp[i] *= repeat;
#endif

  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI Library initialization error! %d\n", __LINE__);
    exit(1);
  }

  compute_matmul(0, nsize, repeat, a, b, c, c_kp);
  compute_matmul(1, nsize, repeat, a, b, c, c_kp);
  compute_matmul(2, nsize, repeat, a, b, c, c_kp);

  printf(
      "-----------------------------------------------------------------------"
      "\n");

}  // main()

void compute_matmul(int version, int nsize, int repeat, double *a, double *b,
                    double *c, double *c_kp) {
  int isec0, iusec0, isec1, iusec1;
  int delta, delta_min, expt;
  double tmatmul, err = 0;
  int i, j;

  float real_time, proc_time, mflops;
  long long values[NUM_EVENTS];
  int events[NUM_EVENTS] = {PAPI_DP_OPS, PAPI_TOT_CYC};

  for (i = 0; i < nsize * nsize; i++) c[i] = 0.0;

  int retval;
  elp_time(&isec0, &iusec0);
  if ((retval = PAPI_start_counters(events, NUM_EVENTS)) != PAPI_OK) {
    fprintf(stderr, "PAPI Start counter error! %d, %d\n", retval, __LINE__);
    exit(1);
  }

  if (version == 0) {
    for (i = 0; i < repeat; i++) matmuldot(nsize, a, b, c);
  } else if (version == 1) {
    for (i = 0; i < repeat; i++) matmuldax(nsize, a, b, c);
  } else {
    for (i = 0; i < repeat; i++) matmulout(nsize, a, b, c);
  }

  elp_time(&isec1, &iusec1);
  if ((retval = PAPI_stop_counters(values, NUM_EVENTS)) != PAPI_OK) {
    fprintf(stderr, "PAPI stop counters error! %d, %d\n", retval, __LINE__);
    exit(1);
  }
  delta = (isec1 - isec0) * 1000000 + iusec1 - iusec0;
  tmatmul = 1.0e-6 * (double)delta;
  mflops = values[0] / delta;

  err = check_err(nsize, c, c_kp);

  printf("%s %10i%10i %14.6f %14.6f %14.2e\n",
         (version == 0 ? "Dot" : (version == 1 ? "Dax" : "Out")), nsize, repeat,
         tmatmul, mflops, err);
}  // compute_matmul()

double check_err(int n, double c[], double c_kp[]) {
  int i, j;
  double err = 0;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) err += fabs(c_kp[i * n + j] - c[i * n + j]);
  return err;
}

void matmuldot(int n, double a[], double b[], double c[]) {
  int i, j, k;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
        c[i * n + j] = c[i * n + j] + a[i * n + k] * b[j * n + k];
}

void matmuldax(int n, double a[], double b[], double c[]) {
  int i, j, k;
  for (i = 0; i < n; i++)
    for (k = 0; k < n; k++)
      for (j = 0; j < n; j++)
        c[i * n + j] = c[i * n + j] + a[i * n + k] * b[k * n + j];
}

void matmulout(int n, double a[], double b[], double c[]) {
  int i, j, k;
  for (k = 0; k < n; k++)
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        c[i * n + j] = c[i * n + j] + a[i * n + k] * b[j * n + k];
}

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
}

void elp_time(int *isec, int *iusec) {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  *isec = tp.tv_sec;
  *iusec = tp.tv_usec;
}
