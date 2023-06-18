/* auxiliary matrix functions
 *      written by Peter Strazdins, RSCS ANU, Sep 2011, for COMP8320
 *      updated by Peter Strazdins, RSCS ANU, Aug 2015, for CISRA Workshop
 */

// matrix are assumed to be column-major

#if 0
#define FDATA float
#define CBLAS_GEMM cblas_sgemm
#define CUBLAS_GEMM cublasSgemm
#define EPSILON 1.0e-06
#else
#define FDATA double
#define CBLAS_GEMM cblas_dgemm
#define CUBLAS_GEMM cublasDgemm
#define EPSILON 1.0e-12
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define V(A, i, j, n) (*((A) + (i) + (j) * (n)))

// sets Mi and Mj to appropriate values for a stable n*n*k matrix multiply
void setMatInit(int k);

// fills m x n matrix A with values of the form  Wi * (i % Mi) + Wj * (j % W)
void initMat(int m, int n, FDATA *A);

// copies  m x n matrix A  to B
void copyMat(int m, int n, FDATA *A, FDATA *B);

// prints A[0:n-1][0:m-1]
void printMat(char *name, int m, int n, FDATA *A);

// predict result of C += alpha A*B where the matrices have been created by
// initMat(m, k, A); initMat(k, n, B); initMat(m, n, C);
void predictDistMatMult(int m, int n, int k, double alpha, FDATA *C);

// compare A & B, return number, sum of and maximum errors
int compMat(int m, int n, FDATA *A, FDATA *B, FDATA *sumErrs, FDATA *maxErrs);

