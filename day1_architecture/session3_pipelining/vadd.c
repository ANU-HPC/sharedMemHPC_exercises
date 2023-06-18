void vadd(int n, double* __restrict__ a, double* __restrict__ b) {
  int k;
  for (k = 0; k < n; k++) {
    a[k] = a[k] + b[k];
  }
}
