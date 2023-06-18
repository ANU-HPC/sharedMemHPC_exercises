#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void prtvec(const char label[], double* vec, int len);
void vadd(int n, double* a, double* b);

int main(int argc, char* argv[]) {
  double* a;
  int nsize, nrpt, i, j;
  char name[21];

  if (argc != 3) {
    printf(" %s Problem_Dimension Number_Repeats \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
    nrpt = atoi(argv[2]);
  }

  a = (double*)malloc(nsize * sizeof(double));
  if (!a) {
    printf(" Allocation failure reduce n %d \n", nsize);
    exit(-1);
  }

  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Program to use hardware performance counters\n");
  printf("Running on:  %20s\n", name);
  printf("Size of Arrays:        %10d\n", nsize);
  printf("Number of Repeats:     %10d\n", nrpt);
  printf("Total Bytes for Arrays %10ld\n\n", 2 * nsize * sizeof(double));
  printf("--------------------------------------------\n");

  for (i = 0; i < nsize; i++) a[i] = 1.0;

#define NUM_EVENTS 1
  int events[NUM_EVENTS] = {PAPI_DP_OPS};
  long long eventValues[NUM_EVENTS];
  long long flpins;
  float mflops;
  printf("Start computation...\n");

  PAPI_library_init(PAPI_VER_CURRENT);

  PAPI_start_counters(events, NUM_EVENTS);
  long long startTime = PAPI_get_virt_usec();
  for (j = 0; j < nrpt; j++) {
    vadd(nsize - 1, &a[1], &a[0]);
  }
  long long stopTime = PAPI_get_virt_usec();
  PAPI_stop_counters(eventValues, NUM_EVENTS);

  flpins = eventValues[0];
  mflops = flpins / (stopTime - startTime);
  printf("Total FP instruction %20lld\n", flpins);
  printf("Avegrage MFLOPS      %20.3f\n", mflops);
  printf("Exec. time (s)   %20.2f\n", (stopTime - startTime) * 1.0e-6);
  //  printf("Last value	       %20.3fs\n", a[nsize-1]);
}  // main()

