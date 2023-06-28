#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "papi.h"

#define DEBUG 0

#define NUM_EVENTS 2

#define BLOCK_SIZE 16

#define MATRIX_BLOCK(nsize, mat, i, j) \
  (&mat[BLOCK_SIZE * i * nsize + j * BLOCK_SIZE])

void print_matrix(const char label[], double *matrix, size_t ncol, size_t nrow);
void elp_time(int *isec, int *iusec);

void cholesky(const size_t nsize, double *l);
void block_cholesky(const size_t n, double *a, const size_t lda);
void block_triangular_solve(const size_t m, const size_t n, const double *a,
                            const size_t lda, double *b, const size_t ldb);
void block_symmetric_rank_k_update(const size_t n, const size_t k,
                                   const double *a, const size_t lda, double *c,
                                   const size_t ldc);
void block_sub_matrix_mul(const size_t m, const size_t n, const size_t k,
                          const double *a, const size_t lda, const double *b,
                          const size_t ldb, double *c, const size_t ldc);
double check_factorization(const size_t n, const double *a, const double *l);

int main(int argc, char *argv[]) {
  size_t nsize;
  char name[21];
  int repeat;

  if (argc != 2) {
    printf(" %s Matrix_Dimension \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
  }

  double *a = (double *)malloc(nsize * nsize * sizeof(double));
  double *l = (double *)malloc(nsize * nsize * sizeof(double));

  if (!(a && l)) {
    printf(" Failed to allocate matrix and vectors of size n %ld \n", nsize);
    exit(-1);
  }
  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Cholesky Factorization Benchmark\n");
  printf("Running on    :  %20s\n", name);
  printf("Size of system:  %20ld\n", nsize);
  printf("--------------------------------------------\n");

  srand(time(NULL));

  // initialize arrays - make symmetric
  for (size_t i = 0; i < nsize; i++) {
    for (size_t j = 0; j <= i; j++) {
      a[i * nsize + j] = 1.0 - (double)rand() / RAND_MAX;
      a[j * nsize + i] = a[i * nsize + j];
    }
    a[i * nsize + i] += nsize;
  }

  if (DEBUG) {
    print_matrix("A", a, nsize, nsize);
  }

  // L = A
  for (size_t i = 0; i < nsize; i++) {
    for (size_t j = 0; j < nsize; j++) {
      l[i * nsize + j] = a[i * nsize + j];
    }
  }

  int isec0, iusec0, isec1, iusec1;
  int delta, delta_min, expt;
  double time, err = 0;

  float real_time, proc_time, mflops;
  long long values[NUM_EVENTS];
  int events[NUM_EVENTS] = {PAPI_DP_OPS, PAPI_TOT_CYC};

  int retval;
  elp_time(&isec0, &iusec0);

  if ((retval = PAPI_start_counters(events, NUM_EVENTS)) != PAPI_OK) {
    fprintf(stderr, "PAPI Start counter error! %d, %d\n", retval, __LINE__);
    exit(1);
  }

  cholesky(nsize, l);

  elp_time(&isec1, &iusec1);

  if ((retval = PAPI_stop_counters(values, NUM_EVENTS)) != PAPI_OK) {
    fprintf(stderr, "PAPI stop counters error! %d, %d\n", retval, __LINE__);
    exit(1);
  }

  delta = (isec1 - isec0) * 1000000 + iusec1 - iusec0;
  time = 1.0e-6 * (double)delta;
  mflops = 1.0 / 3.0 * nsize * nsize * nsize / delta;
  err = check_factorization(nsize, a, l);

  printf("\n\n");
  printf("      size       time(s)     MFLOP/s       Error\n");
  printf(
      "-----------------------------------------------------------------------"
      "\n");
  printf("%10ld %12.6f %12.2e %11.3e\n", nsize, time, mflops, err);

  if (DEBUG) {
    print_matrix("L", l, nsize, nsize);
  }

  printf(
      "-----------------------------------------------------------------------"
      "\n");

  free(l);
  free(a);

}  // main()

/*
 * Compute the Cholesky factorization of matrix L, overwriting the original
 * matrix. The factorization is computed using a right-looking blocked
 * Cholesky decomposition.
 */
void cholesky(const size_t nsize, double *l) {
  // set upper triangle to zero
  for (size_t i = 0; i < nsize; i++) {
    for (size_t j = i + 1; j < nsize; j++) {
      l[i * nsize + j] = 0.0;
    }
  }

  int num_blocks = (nsize + BLOCK_SIZE - 1) / BLOCK_SIZE;

  for (int k = 0; k < num_blocks; k++) {
    size_t k_height = k < num_blocks - 1
                          ? BLOCK_SIZE
                          : nsize - (BLOCK_SIZE * (num_blocks - 1));
    block_cholesky(k_height, MATRIX_BLOCK(nsize, l, k, k),
                   nsize);  // A[k][k] = sqrt(A[k][k])

    for (int m = k + 1; m < num_blocks; m++) {
      size_t m_width = m < num_blocks - 1
                           ? BLOCK_SIZE
                           : nsize - (BLOCK_SIZE * (num_blocks - 1));
      block_triangular_solve(m_width, k_height, MATRIX_BLOCK(nsize, l, k, k),
                             nsize, MATRIX_BLOCK(nsize, l, m, k),
                             nsize);  // A[m][k] = A[m,k] A[k][k]^-1
    }

    for (int m = k + 1; m < num_blocks; m++) {
      size_t m_width = m < num_blocks - 1
                           ? BLOCK_SIZE
                           : nsize - (BLOCK_SIZE * (num_blocks - 1));
      block_symmetric_rank_k_update(
          m_width, k_height, MATRIX_BLOCK(nsize, l, m, k), nsize,
          MATRIX_BLOCK(nsize, l, m, m), nsize);  // A[m][m] -= A[m][k] A[m][k]^T

      for (int n = k + 1; n < m; n++) {
        size_t n_width = n < num_blocks - 1
                             ? BLOCK_SIZE
                             : nsize - (BLOCK_SIZE * (num_blocks - 1));
        block_sub_matrix_mul(
            m_width, n_width, k_height, MATRIX_BLOCK(nsize, l, m, k), nsize,
            MATRIX_BLOCK(nsize, l, n, k), nsize, MATRIX_BLOCK(nsize, l, m, n),
            nsize);  // A[m][n] -= A[m][k] A[n,k]^T
      }
    }
  }

}  // cholesky ()

/*
 * Update A = cholesky factorization of A where A is an n by n matrix
 * @param n number of rows and columns of A
 * @param a matrix A (n by n)
 * @param lda stride of leading dimension of a
 * @see BLAS dpotrf
 */
void block_cholesky(const size_t n, double *a, const size_t lda) {
  for (size_t i = 0; i < n; i++) {
    a[i * lda + i] = sqrt(a[i * lda + i]);
    double invd = 1.0 / a[i * lda + i];
    for (size_t j = i + 1; j < n; j++) {
      a[j * lda + i] = a[j * lda + i] * invd;
    }
    for (size_t j = i + 1; j < n; j++) {
      for (int k = i + 1; k <= j; k++) {
        a[j * lda + k] -= a[j * lda + i] * a[k * lda + i];
      }
    }
  }
}

/*
 * Solves the matrix equation X A^T = B where X and B are m by n matrices,
 * and overwrites the matrix B with X.
 * @param m number of rows of matrix B
 * @param n number of columns of matrix B
 * @param a matrix A, dimensions n by n
 * @param lda stride of leading dimension of A
 * @param b matrix B (also X), dimensions m by n
 * @param ldb stride of leading dimension of B
 * @see BLAS dtrsm
 */
void block_triangular_solve(const size_t m, const size_t n, const double *a,
                            const size_t lda, double *b, const size_t ldb) {
  for (size_t k = 0; k < n; k++) {
    double tmp = 1.0 / a[k * lda + k];
    for (size_t i = 0; i < m; i++) {
      b[i * ldb + k] = tmp * b[i * ldb + k];
    }
    for (size_t j = k + 1; j < n; j++) {
      if (a[j * lda + k] != 0.0) {
        double tmp = a[j * lda + k];
        for (size_t i = 0; i < m; i++) {
          b[i * ldb + j] = b[i * ldb + j] - tmp * b[i * ldb + k];
        }
      }
    }
  }
}

/*
 * Performs the symmetric rank-K update C -= A * A^T
 * @param n number of rows and columns in C and number of rows in A
 * @param k number of columns in A
 * @param a matrix A, dimensions n by k
 * @param lda stride of leading dimension of A
 * @param c matrix C, dimensions n by n
 * @param ldc stride of leading dimension of C
 * @see BLAS dsyrk
 */
void block_symmetric_rank_k_update(const size_t n, const size_t k,
                                   const double *a, const size_t lda, double *c,
                                   const size_t ldc) {
  for (size_t j = 0; j < n; j++) {
    for (size_t l = 0; l < k; l++) {
      double tmp = a[j * lda + l];
      for (size_t i = j; i < n; i++) {
        c[i * ldc + j] -= tmp * a[i * lda + l];
      }
    }
  }
}

/*
 * Updates C -= A * B^T
 * @param m number of rows of A and C
 * @param n number of columns of C and rows of B
 * @param k number of columns of A and B
 * @param a matrix A, dimensions m by k
 * @param lda stride of leading dimension of A
 * @param b matrix B, dimensions n by k
 * @param lda stride of leading dimension of B
 * @param c matrix C, dimensions m by n
 * @param ldc stride of leading dimension of C
 * @see BLAS dgemm
 */
void block_sub_matrix_mul(const size_t m, const size_t n, const size_t k,
                          const double *a, const size_t lda, const double *b,
                          const size_t ldb, double *c, const size_t ldc) {
  for (size_t i = 0; i < m; i++)
    for (size_t j = 0; j < n; j++)
      for (size_t l = 0; l < k; l++)
        c[i * ldc + j] = c[i * ldc + j] - a[i * lda + l] * b[j * ldb + l];
}

/*
 * Compute the error of the Cholesky factorization L of matrix A via
 * matrix-vector multiplication. Generates a random vector v, returns
 * max(abs(LL^Tv - Av)).
 */
double check_factorization(size_t n, const double *a, const double *l) {
  double max_err = 0.0;
  double *v = malloc(n * sizeof(double));
  double *err = malloc(n * sizeof(double));
  double *temp = malloc(n * sizeof(double));

  for (int i = 0; i < n; i++) {
    v[i] = (double)(rand()) / RAND_MAX * 2.0 - 1.0;
  }

  // temp = L^Tv
  for (size_t i = 0; i < n; i++) {
    temp[i] = 0.0;
    for (size_t j = 0; j < n; j++) {
      temp[i] += l[j * n + i] * v[j];
    }
    if (isnan(temp[i])) return INFINITY;
  }
  // err = LL^Tv
  for (size_t i = 0; i < n; i++) {
    err[i] = 0.0;
    for (size_t j = 0; j < n; j++) {
      err[i] += l[i * n + j] * temp[j];
    }
  }

  // err -= Av
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      err[i] -= a[i * n + j] * v[j];
    }
    max_err = fmax(max_err, fabs(err[i]));
  }

  free(v);
  free(err);
  free(temp);
  return max_err;
}

#define PRINT_COLS 5
void print_matrix(const char label[], double *matrix, size_t ncol,
                  size_t nrow) {
  size_t icount, row, col;
  printf("\n print_matrix : %-40s \n", label);
  icount = 0;
  for (row = 0; row < nrow; row++) {
    printf("%2ld ", row);
    for (col = 0; col < ncol; col++) {
      printf("%11.3e", matrix[icount]);
      icount++;
      if (col % PRINT_COLS == (PRINT_COLS - 1) && col < (ncol - 1))
        printf("\n   ");
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
