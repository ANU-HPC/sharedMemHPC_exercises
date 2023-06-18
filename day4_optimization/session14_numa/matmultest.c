/* matrix multiply test harness
 * 	written by Peter Strazdins, RSCS ANU, Sep 2011, for COMP8320 Lab Ex 7
 *      updated by Peter Strazdins, RSCS ANU, Aug 2015, for CISRA Workshop
 *      ported to Intel MKL by Peter Strazdins, RSCS ANU, Jan 2020 for ASD Wksp
 */
#define BLAS_AVAIL

#include <assert.h>
#include <math.h> /* fabs() */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> /* gettimeofday() */
#include <unistd.h>   /* getopt() */
#ifdef BLAS_AVAIL
#include <mkl.h>
#endif
#include "mat.h"
#include "matmult.h"

#define USAGE "matmultest [-p] [-v v] N K"
#ifdef BLAS_AVAIL
#define DEFAULTS "v=0"
#else
#define DEFAULTS "v=1"
#endif
#define OPTCHARS "pv:"

static int N, K, printData, v = 0;

static char *EFFECT[] = {
    "perform N x N x K matrix multiply; use -p to print arrays"};

// print a usage message for this program and exit with a status of 1
static void usage(char *msg) {
  int e;
  if (msg != NULL) printf("%s\n", msg);
  printf("usage: %s\n\tdefaults: %s\n", USAGE, DEFAULTS);
  for (e = 0; e < sizeof(EFFECT) / sizeof(EFFECT[0]); e++)
    printf("\t%s\n", EFFECT[e]);
  exit(1);
}  // usage()

// process argv[0..argc-1] and set corresponding global variables
void getArgs(int argc, char *argv[]) {
  extern char *optarg;  // points to option argument (for -p option)
  extern int optind;    // index of last option parsed by getopt()
  extern int opterr;
  char optchar;  // option character returned my getopt()
  opterr = 0;    // suppress getopt() error message for invalid option

  printData = 0;
#ifdef BLAS_AVAIL
  v = 0;
#else
  v = 1
#endif
  while ((optchar = getopt(argc, argv, OPTCHARS)) != -1) {
    switch (optchar) {
      case 'v':
        if (sscanf(optarg, "%d", &v) != 1) usage("invalid number in v");
        break;
      case 'p':
        printData = 1;
        break;
      default:
        usage("unknown option");
    }
  }  // while (optchar...)

  if (optind >= argc) usage("N is missing");
  if (sscanf(argv[optind], "%d", &N) != 1) usage("invalid number for N");
  if (optind + 1 >= argc) usage("K is missing");
  if (sscanf(argv[optind + 1], "%d", &K) != 1) usage("invalid number for K");
}  // getArgs()

static struct timeval tvStart;
void startTimer() {
  struct timezone tz;
  gettimeofday(&tvStart, &tz);
}

double stopTimer() {
  struct timezone tz;
  struct timeval tvEnd;
  gettimeofday(&tvEnd, &tz);
  return ((double)(tvEnd.tv_sec - tvStart.tv_sec +
                   (tvEnd.tv_usec - tvStart.tv_usec) / 1.0E06));
}

/*************** main program **********************************/
int main(int argc, char *argv[]) {
  FDATA *A, *B, *C, *D;  // D is used for checking
  double timeCompute;
  int nErrs;
  FDATA sumErrs, maxErr;

  getArgs(argc, argv);
  printf("compute a %dx%dx%d matrix multiply (version %d)\n", N, N, K, v);

  A = (FDATA *)malloc(N * K * sizeof(FDATA));
  B = (FDATA *)malloc(K * N * sizeof(FDATA));
  C = (FDATA *)malloc(N * N * sizeof(FDATA));
  D = (FDATA *)malloc(N * N * sizeof(FDATA));
  assert(A != NULL && B != NULL && C != NULL && D != NULL);

  setMatInit(K);
  initMat(N, N, D);
  copyMat(N, N, D, C);
  initMat(N, K, A);
  initMat(K, N, B);

  if (printData) {
    printMat("A", N, K, A);
    printMat("B", K, N, B);
    printMat("C", N, N, C);
  }

  startTimer();
  switch (v) {
    case 0:
#ifdef BLAS_AVAIL
      CBLAS_GEMM(CblasColMajor, CblasNoTrans, CblasNoTrans, N, N, K, 1.0, A, N,
                 B, K, 1.0, C, N);
      break;
#endif
    default:
      matMult(v, N, K, A, B, C);
      break;
  }
  timeCompute = stopTimer();

  printf("computed multiply in %Gs, %.2f GFLOPs\n", timeCompute,
         2.0E-9 * N * N * K / timeCompute);
  if (printData) {
    printMat("C", N, N, C);
  }

  predictDistMatMult(N, N, K, 1.0, D);
  nErrs = compMat(N, N, C, D, &sumErrs, &maxErr);
  if (nErrs > 0)
    printf("%d errors in matrix multiply: sum %f, max %f\n", nErrs, sumErrs,
           maxErr);
  else
    printf("result passed test!\n");

  free(A);
  free(B);
  free(C);
  free(D);

  return (0);
}  // main()
