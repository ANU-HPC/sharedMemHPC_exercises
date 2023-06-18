#define _DEFAULT_SOURCE
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <malloc.h>
#include <unistd.h>

#define PAPI_COUNT_ARRAY_LEN 2
#define NUM_EVENTS 2

int main(int argc, char** argv) {
  int       nsize, nrpt;
  double    sum, *b, *a;
  long long itercount;
  char      name[30];

  if (argc != 3) {
    printf(" %s Problem_Dimension Number_Repeats \n",argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
    nrpt  = atoi(argv[2]);
  }

  a = (double*) malloc(nsize*sizeof(double));
  b = (double*) malloc(nsize*sizeof(double));
  if (!(a && b)){
    printf(" Allocation failure reduce n %d \n",nsize);
    exit(-1);
  }
  gethostname(name, 20);
  printf("--------------------------------------------\n");
  printf("Program to use hardware performance counters\n");
  printf("Running on:  %20s\n",name);
  printf("Size of Arrays:        %10d\n",nsize);
  printf("Number of Repeats:     %10d\n",nrpt);
  printf("Total Bytes for Arrays %10ld\n\n",2*nsize*sizeof(double));
  printf("--------------------------------------------\n");

  //---------------------- PAPI Init  ---------------------------------//
  int Events[NUM_EVENTS] = {PAPI_DP_OPS, PAPI_TOT_CYC};
  long long event_values[NUM_EVENTS];
  int retval = 0;
  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
    fprintf(stderr,"PAPI Library initialization error! %d\n",  __LINE__);
    exit(1);
  }

  printf("Start computation...\n");
  float real_time, proc_time, total_proc_time;
  long long flpins;
  float mflops;
  for (int i = 0; i < nsize; i++) {
    b[i] = 0.3 + 0.001*i + 0.00001*i;
    a[i] = (double) rand() / (double) RAND_MAX;
  }

  //------------------- Measured loop ---------------------------------//
  itercount = ((long long)nrpt)*nsize;
  long long values[PAPI_COUNT_ARRAY_LEN];

  //loop one
  if ((retval = PAPI_start_counters(Events, NUM_EVENTS)) != PAPI_OK) {
    if (retval == PAPI_ENOEVNT) {
      fprintf(stderr,"At least one selected PAPI counter is not available on the underlying hardware!\n");
    } else {
      fprintf(stderr,"PAPI Start counter error! %d, %d\n",  retval, __LINE__);
    }
    exit(1);
  }

  long long StartTime = PAPI_get_virt_usec();
  sum = 0.0;
  for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
      //-- loop one --//
      sum = sum + (a[k]*b[k]);
    } // for k
  } // for j

  long long StopTime = PAPI_get_virt_usec();
  /* Stop counting events */
  if ((retval = PAPI_stop_counters(event_values, NUM_EVENTS)) != PAPI_OK){
    fprintf(stderr,"PAPI stop counters error! %d, %d\n", retval, __LINE__);
    exit(1);
  }
    
  printf("\nLoop one         \n\n");
  printf("Total Iterations %20lld\n",itercount);
  printf("PAPI_DP_OPS      %20lld\n", event_values[0]);
  printf("PAPI_TOT_CYC     %20lld\n", event_values[1]);
  printf("Inst per cycle   %20.2f\n", (double)event_values[0]/(double)event_values[1]);
  printf("MFLOPS           %20.2f\n", (double) event_values[0]/ (double)(StopTime - StartTime));
  printf("Exec. time (s)   %20.2f\n", (StopTime - StartTime)*1.0e-6);
  printf("sum = %14.7e\n", sum);

  //loop two 
  if ((retval = PAPI_start_counters(Events, NUM_EVENTS)) != PAPI_OK) {
    if (retval == PAPI_ENOEVNT) {
      fprintf(stderr,"At least one selected PAPI counter is not available on the underlying hardware!\n");
    } else {
      fprintf(stderr,"PAPI Start counter error! %d, %d\n",  retval, __LINE__);
    }
    exit(1);
  }

  StartTime = PAPI_get_virt_usec();
  sum = 0.0;
  for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
      //-- loop two --//
      sum = sum + a[k] * (a[k]+b[k]);
    } // for k
  } // for j
  StopTime = PAPI_get_virt_usec();

  /* Stop counting events */
  if ((retval = PAPI_stop_counters(event_values, NUM_EVENTS)) != PAPI_OK){
    fprintf(stderr,"PAPI stop counters error! %d, %d\n", retval, __LINE__);
    exit(1);
  }
  
  printf("\nLoop two         \n\n");
  printf("Total Iterations %20lld\n",itercount);
  printf("PAPI_DP_OPS      %20lld\n", event_values[0]);
  printf("PAPI_TOT_CYC     %20lld\n", event_values[1]);
  printf("Inst per cycle   %20.2f\n", (double) event_values[0]/ (double)event_values[1]);
  printf("MFLOPS           %20.2f\n", (double) event_values[0]/ (double)(StopTime - StartTime));
  printf("Exec. time (s)   %20.2f\n", (StopTime - StartTime)*1.0e-6);
  printf("sum = %14.7e\n", sum);

  //loop three 
  if ((retval = PAPI_start_counters(Events, NUM_EVENTS)) != PAPI_OK) {
    if (retval == PAPI_ENOEVNT) {
      fprintf(stderr,"At least one selected PAPI counter is not available on the underlying hardware!\n");
    } else {
      fprintf(stderr, "PAPI Start counter error! %d, %d\n", retval, __LINE__);
    }
    exit(1);
  }

  StartTime = PAPI_get_virt_usec();
  sum = 0.0;
  for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
      //-- loop three --//
      if (a[k] > 0.50) {
	sum = sum + (a[k]*b[k]);
      } else {
	sum = sum - (a[k]*b[k]);
      }
    } // for k
  } // for j
  StopTime = PAPI_get_virt_usec();
  
  if ((retval = PAPI_stop_counters(event_values, NUM_EVENTS)) != PAPI_OK){
    fprintf(stderr,"PAPI stop counters error! %d, %d\n", retval, __LINE__);
    exit(1);
  }

  printf("\nLoop three         \n\n");
  printf("Total Iterations %20lld\n",itercount);
  printf("PAPI_DP_OPS      %20lld\n", event_values[0]);
  printf("PAPI_TOT_CYC     %20lld\n", event_values[1]);
  printf("Inst per cycle   %20.2f\n", (double)event_values[0]/(double)event_values[1]);
  printf("MFLOPS           %20.2f\n", (double) event_values[0]/ (double)(StopTime - StartTime));
  printf("Exec. time (s)   %20.2f\n", (StopTime - StartTime)*1.0e-6);
  printf("sum = %14.7e\n", sum);

  /* clean up */ 
  printf("--------------------------------------------\n");

  free(a);
  free(b);    
  
  return 0;
} //main()

