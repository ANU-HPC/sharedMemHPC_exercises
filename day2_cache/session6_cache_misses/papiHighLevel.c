#define _DEFAULT_SOURCE
#include <malloc.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_EVENTS 2

int run(int nsize, int nrpt, int loop_number);

int main(int argc, char** argv) {
  int nsize, nrpt;
  long long itercount;

  if (argc != 3) {
    printf(" %s Problem_Dimension Number_Repeats \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
    nrpt = atoi(argv[2]);
  }

  char name[30];
  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Program to use hardware performance counters\n");
  printf("Running on:  %20s\n", name);
  printf("Size of Arrays:        %10d\n", nsize);
  printf("Number of Repeats:     %10d\n", nrpt);
  printf("Total Bytes for Arrays %10ld\n", 2 * nsize * sizeof(double));
  printf("--------------------------------------------\n\n");

  run(nsize, nrpt, 0);
  run(nsize, nrpt, 1);
  run(nsize, nrpt, 2);
}  // main()

int run(int nsize, int nrpt, int loop_number) {
  printf("Start computation... Loop %d\n", loop_number + 1);

  long long itercount;
  double sum, *b, *a;

  a = (double*)malloc(nsize * sizeof(double));
  b = (double*)malloc(nsize * sizeof(double));
  if (!(a && b)) {
    printf(" Allocation failure reduce n %d \n", nsize);
    exit(-1);
  }

  int count = 0;
  for (int i = 0; i < nsize; i++) {
    b[i] = 0.3 + 0.001 * i + 0.00001 * i;
    a[i] = (double)rand() / (double)RAND_MAX;
  }

  //-------------------------------------------------------------------//
  //---------------------- PAPI Init  ---------------------------------//
  //-------------------------------------------------------------------//
  int Events[NUM_EVENTS] = {PAPI_DP_OPS, PAPI_TOT_CYC};
  long long values[NUM_EVENTS];
  int retval = 0;
  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI Library initialization error! %d\n", __LINE__);
    exit(1);
  }

  /* Start counting events */
  if ((retval = PAPI_start_counters(Events, NUM_EVENTS)) != PAPI_OK) {
    if (retval == PAPI_ENOEVNT) {
      fprintf(stderr,
              "At least one selected PAPI counter is not available on the "
              "underlying hardware!\n");
    } else {
      fprintf(stderr, "PAPI Start counter error! %d, %d\n", retval, __LINE__);
    }
    exit(1);
  }
  long long StartTime = PAPI_get_virt_usec();

  //-------------------------------------------------------------------//
  //------------------- Measured loop ---------------------------------//
  //-------------------------------------------------------------------//
  if (loop_number == 0) {
    sum = 0.0;
    for (int j = 0; j < nrpt; j++) {
      for (int k = 0; k < nsize; k++) {
        //-- loop one --//
        sum = sum + (a[k] * b[k]);
      }
    }  // for nrpt
  } else if (loop_number == 1) {
    sum = 0.0;
    for (int j = 0; j < nrpt; j++) {
      for (int k = 0; k < nsize; k++) {
        //-- loop two --//
        sum = sum + a[k] * (a[k] + b[k]);
      }
    }  // for nrpt
  } else if (loop_number == 2) {
    sum = 0.0;
    for (int j = 0; j < nrpt; j++) {
      for (int k = 0; k < nsize; k++) {
        //-- loop three --//
        if (a[k] > 0.5) {
          sum = sum + (a[k] * b[k]);
        } else {
          sum = sum - (a[k] * b[k]);
        }
      }
    }  // for nrpt
  } else {
    fprintf(stderr, "Invalid loop number!\n");
    exit(1);
  }

  //-------------------------------------------------------------------//
  //---------------------- PAPI stop counters----------------------------//
  //-------------------------------------------------------------------//
  long long StopTime = PAPI_get_virt_usec();
  if ((retval = PAPI_stop_counters(values, NUM_EVENTS)) != PAPI_OK) {
    fprintf(stderr, "PAPI stop counters error! %d, %d\n", retval, __LINE__);
    exit(1);
  }

  //-------------------------------------------------------------------//
  //---------------------- print values    ----------------------------//
  //-------------------------------------------------------------------//
  itercount = ((long long)nrpt) * nsize;
  printf("Total Iterations %20lld\n", itercount);
  printf("Exec. time (s)   %20.2f\n", (StopTime - StartTime) * 1.0e-6);
  printf("PAPI_DP_OPS      %20lld\n", values[0]);
  printf("PAPI_TOT_CYC     %20lld\n", values[1]);
  printf("MFLOPS           %20.2f\n",
         (double)values[0] / (double)(StopTime - StartTime));
  printf("Inst per cycle   %20.2f\n", (double)values[0] / (double)values[1]);

  printf("sum = %14.7e\n", sum);
  printf("\n");

  free(a);
  free(b);

  return 0;
}  // run()
