#include "myvec.h"

void mydot(double *c, int n, double a[], int stda, double b[], int stdb) {
  int j;
  for (j = 0; j < n; j++) {
    *c += a[j * stda] * b[j * stdb];
  }
}

void mydaxpy(int n, double b, double c[], int stda, double a[], int stdx) {
  int j;
  for (j = 0; j < n; j++) {
    c[j * stda] += a[j * stdx] * b;
  }
}

// unrolled versions
void mydot_u2(double c[], int n, double a[], int stda, double b[], int stdb,
              int offset) {
  int j;
  for (j = 0; j < n; j++) {
    c[0] += a[j * stda] * b[j * stdb];
    c[1] += a[j * stda + offset] * b[j * stdb];
  }
}

void mydaxpy_u2(int n, double b[], double c[], int stda, double a[], int stdx,
                int offset) {
  int j;
  for (j = 0; j < n; j++) {
    c[j * stda] += a[j * stdx] * b[0] + a[j * stdx + offset] * b[1];
  }
}
