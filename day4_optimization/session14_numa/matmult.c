/* matrix multiply functions CISRA Workshop 2015
 */
#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "mat.h" /*FDATA*/

// vanilla matrix multiply
static void matMult_1(int N, int K, FDATA *A, FDATA *B, FDATA *C) {
  int i, j, k;
  for (j = 0; j < N; j++) {
    for (i = 0; i < N; i++) {
      register FDATA t = V(C, i, j, N);
      for (k = 0; k < K; k++) t += V(A, i, k, N) * V(B, k, j, K);
      V(C, i, j, N) = t;
    }  // for (j...)
  }    // for (i...)
}  // matMult_1()

static void matMult_2(int N, int K, FDATA *A, FDATA *B, FDATA *C) {
}  // matMult_2()

static void matMult_3(int N, int K, FDATA *A, FDATA *B, FDATA *C) {
}  // matMult_12)

static void matMult_4(int N, int K, FDATA *A, FDATA *B, FDATA *C) {
}  // matMult_3()

static void matMult_5(int N, int K, FDATA *A, FDATA *B, FDATA *C) {
}  // matMult_4()

void matMult(int v, int N, int K, FDATA *A, FDATA *B, FDATA *C) {
  switch (v) {
    case 1:
      matMult_1(N, K, A, B, C);
      break;
    case 2:
      matMult_2(N, K, A, B, C);
      break;
    case 3:
      matMult_3(N, K, A, B, C);
      break;
    case 4:
      matMult_4(N, K, A, B, C);
      break;
    case 5:
      matMult_5(N, K, A, B, C);
      break;
    default:
      printf("mztmult(): version %d not yet implemented\n", v);
      exit(1);
  }
}  // matMult()
