#include "vector_add.h"

#include <omp.h>
#include <stdio.h>

#include "vector_base.h"

/*
 * Vector add
 * standard implementation
 *
 */
void VectorAdd(const float *__restrict__ a, const float *__restrict__ b,
               float *__restrict__ c, const size_t size, const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
#pragma omp parallel for shared(a, b, c) schedule(static)
    for (size_t i = 0; i < size; i++) {
      c[i] = a[i] + b[i];
    }
  }
}

/*
 * Vector add
 * Task implementation
 *
 */

void VectorAddTask(const float *__restrict__ a, const float *__restrict__ b,
                   float *__restrict__ c, const size_t size,
                   const size_t nrpt) {}

/*
 * Execute simple vector adds and compare run time
 *
 */
void ExecuteVectorAdds(const size_t size, const size_t nrpt) {
  float *a = NULL;
  float *b = NULL;
  float *c = NULL;

  AllocateMemory(&a, size);
  AllocateMemory(&b, size);
  AllocateMemory(&c, size);

  FillArray(a, size);
  FillArray(b, size);
  FillArray(c, size);

  double StartTime;
  double StopTime;

#ifndef PERFORMANCE
  PrintVector(a, size, "a");
  PrintVector(b, size, "b");
#endif

  StartTime = omp_get_wtime();
  VectorAdd(a, b, c, size, nrpt);
  StopTime = omp_get_wtime();
  printf("Vector add     :  %fs\n", StopTime - StartTime);

#ifndef PERFORMANCE
  PrintVector(c, size, "c     ");
  FillArray(c, size);
#endif

  StartTime = omp_get_wtime();
  VectorAddTask(a, b, c, size, nrpt);
  StopTime = omp_get_wtime();
  printf("Vector add task:  %fs\n", StopTime - StartTime);

#ifndef PERFORMANCE
  PrintVector(c, size, "c-task");
#endif

  FreeMemory(a);
  FreeMemory(b);
  FreeMemory(c);

}
