/* auxiliary  matrix functions
 *      written by Peter Strazdins, RSCS ANU, Sep 2011, for COMP8320
 *      updated by Peter Strazdins, RSCS ANU, Aug 2015, for CISRA Workshop
 */

#include "mat.h"

#include <math.h> /* fabs(), sqrt() */
#include <stdio.h>
#include <stdlib.h>

/* idea: set matrix elements to be linear combinations of i & j, as this
   helps with debugging. However, for large matrices, this causes elements to
   grow too large, resulting in the accumulated roundoff being large enough to
   be considered a genuine error. So, we set a modulus on the matrix indices
   which should avoid this.
*/
#define MANTISSA_W 24 /* width of float mantissa */
#define Wi 2
#define Wj 1
#define V_M(K, W) ((1 << (MANTISSA_W / 2)) / (K) / ((W) + 1))
#define DEF_K 256 /* assumed value of K for matrix multiply*/

static int Mi = 0, Mj = 0;

void setMatInit(int k) {
  k = (int)sqrt((double)k);
  Mi = V_M(k, Wj), Mj = V_M(k, Wi);
}

void initMat(int m, int n, FDATA *A) {
  int i, j;
  if (Mi == 0) setMatInit(DEF_K);
  for (j = 0; j < n; j++)
    for (i = 0; i < m; i++) V(A, i, j, m) = Wi * (i % Mi) + Wj * (j % Mj);
}  // initMat()

void printMat(char *name, int m, int n, FDATA *A) {
  int i, j;
  for (i = 0; i < m; i++) {
    if (i == 0)
      printf("%s: ", name);
    else
      printf("   ");
    for (j = 0; j < n; j++) printf("\t%.1f", V(A, i, j, m));
    printf("\n");
  }
}  // printMat()

void copyMat(int m, int n, FDATA *A, FDATA *B) {
  int i, j;
  for (j = 0; j < n; j++)
    for (i = 0; i < m; i++) V(B, i, j, m) = V(A, i, j, m);
}  // copyMat()

/* pre:  K>=0; m>0
 * post: returns sum: 0 <= k < K: k%m
 */
static int ModSumK(int K, int m) {
  int k, s = 0, km = 0;
  for (k = 0; k < K; k++) {
    s += km;
    km++;
    if (km == m) km = 0;
  }
  return (s);
} /* ModSumK() */

/* pre:  K>=0; my, mx>0
 * post: returns sum: 0 <= k < K: ((k%my) * (k%mx))
 */
static double ModSumK2(int K, int my, int mx) {
  int k;
  double s = 0.0;
  int kmx = 0, kmy = 0;
  for (k = 0; k < K; k++) {
    s += (double)kmy * (double)kmx;
    kmx++;
    if (kmx == mx) kmx = 0;
    kmy++;
    if (kmy == my) kmy = 0;
  }
  return (s);
}

void predictDistMatMult(int m, int n, int k, double alpha, FDATA *C) {
  int i, j;
  double D00 = alpha * Wj * Wi * ModSumK2(k, Mj, Mi);
  double RInc = alpha * Wi * Wi * ModSumK(k, Mi);
  double CInc = alpha * Wj * Wj * ModSumK(k, Mj);
  double RCInc = alpha * Wi * Wj * k;
  double *jMs, *WjMs;
  jMs = (double *)malloc(2 * n * sizeof(double));
  WjMs = jMs + n;
  for (j = 0; j < n; j++) {
    double jM = j % Mj;
    jMs[j] = jM;
    WjMs[j] = Wj * jM;
  }
  for (i = 0; i < m; i++) {
    int iM = i % Mi;
    double DI0 = D00 + RInc * iM, DIInc = CInc + RCInc * iM, WiM = Wi * iM;
    for (j = 0; j < n; j++) {
      double Dij = WiM + WjMs[j] + DI0 + DIInc * jMs[j];
      V(C, i, j, m) = Dij;
    }
  }
  free(jMs);
}  // predictDistMatMult()

int compMat(int m, int n, FDATA *A, FDATA *B, FDATA *sumErrs, FDATA *maxErrs) {
  int i, j, nErrs = 0;
  FDATA s = 0.0, mx = 0.0;
  for (j = 0; j < n; j++) {
    for (i = 0; i < m; i++) {
      FDATA v = fabs(V(B, i, j, m) - V(A, i, j, m));
      if (v > mx) mx = v;
      s += v;
      nErrs += (v > EPSILON);
    }
  }
  *sumErrs = s, *maxErrs = mx;
  return (nErrs);
}  // compMat
