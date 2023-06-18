#define _DEFAULT_SOURCE
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <malloc.h>
#include <unistd.h>

#define PAPI_COUNT_ARRAY_LEN 2
int main(int argc, char** argv) {
  int       nsize, nrpt;
  double    sum, *b, *a;
  long long itercount;
  char      name[30];
   
  if (argc != 3) {
    printf(" %s Problem_Dimension Number_Repeats \n", argv[0]);
    return -1;
  } else {
    nsize = atoi(argv[1]);
    nrpt  = atoi(argv[2]);
  }
       
  a = (double*) malloc(nsize*sizeof(double));
  b = (double*) malloc(nsize*sizeof(double));
  if (!(a && b)) {
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

  printf("Start computation...\n");
  
  //----- PAPI_Profiler Example-----//
  float real_time, proc_time, total_proc_time;
  long long flpins;
  float mflops;
   
  for (int i = 0; i < nsize; i++) {
    b[i] = 0.3 + 0.001*i + 0.00001*i;
    a[i] = (double) rand() / (double) RAND_MAX;
  }

  //-------------------------------------------------------------------//
  //------------------- Measured loop ---------------------------------//
  //-------------------------------------------------------------------//
    
  itercount = ((long long)nrpt)*nsize;
  long long values[PAPI_COUNT_ARRAY_LEN];

  //loop one

  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) != PAPI_OK)
    perror("Error in PAPI_flops, call 1, loop 1");
  sum = 0.0;      
  for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
      //-- loop one --//
      sum = sum + (a[k]*b[k]);
    }// for k
  }// for j

  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) != PAPI_OK)
    perror("Error in PAPI_flops, call 2, loop 1");
  if (PAPI_stop_counters(values, PAPI_COUNT_ARRAY_LEN) != PAPI_OK)
    perror("Error in PAPI_stop_counters, loop 1"); 
    
  printf("\nLoop one               \n\n");
  printf("Total Iterations     %20lld\n", itercount);
  printf("Total FP instruction %20lld\n", flpins);
  printf("Average MFLOPS       %20.3f\n", mflops );
  printf("Process time         %20.3fs\n", proc_time);
  printf("Wall time            %20.3fs\n", real_time);
  printf("sum = %14.7e\n", sum);
   
  //loop two 
  
  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) != PAPI_OK)
    perror("Error in PAPI_flops, call 1, loop 2");

  sum = 0.0;
  for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
      //-- loop two --//
      sum = sum + a[k] * (a[k]+b[k]);
    }// for k
  }// for j

  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) != PAPI_OK)
    perror("Error in PAPI_flops, call 2, loop 2");
  if (PAPI_stop_counters( values, PAPI_COUNT_ARRAY_LEN ) != PAPI_OK )
    perror("Error in PAPI_stop_counters, loop 2"); 

  printf("\nLoop two               \n\n");
  printf("Total Iterations     %20lld\n", itercount);
  printf("Total FP instruction %20lld\n", flpins);
  printf("Average MFLOPS       %20.3f\n", mflops );
  printf("Process time         %20.3fs\n", proc_time);
  printf("Wall time            %20.3fs\n", real_time);
  printf("sum = %14.7e\n", sum);
   
  //loop three 

  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) != PAPI_OK)
    perror("Error in PAPI_flops, call 1, loop 3");

  sum = 0.0;
  for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
      //-- loop three --//
      if (a[k] > 0.50) {
	sum = sum + (a[k]*b[k]);
      } else {
	sum = sum - (a[k]*b[k]);
      }
    }// for k
  }// for j

  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) != PAPI_OK)
    perror("Error in PAPI_flops, call 2, loop 3");
  if (PAPI_stop_counters( values, PAPI_COUNT_ARRAY_LEN) != PAPI_OK )
    perror("Error in PAPI_stop_counters, loop 3"); 

  printf("\nLoop three             \n\n");
  printf("Total Iterations     %20lld\n", itercount);
  printf("Total FP instruction %20lld\n", flpins);
  printf("Average MFLOPS       %20.3f\n", mflops );
  printf("Process time         %20.3fs\n", proc_time);
  printf("Wall time            %20.3fs\n", real_time);
  printf("sum = %14.7e\n", sum);
   
  /* clean up */ 
  printf("--------------------------------------------\n");
  free(a);
  free(b);
  return 0;
} //main()

