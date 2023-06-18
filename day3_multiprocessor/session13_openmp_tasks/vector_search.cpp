#include "vector_search.h"

#include <omp.h>
#include <stdio.h>

#include "vector_base.h"

const long KEY_NOT_FOUND = -1;

/*
 * Binary vector search - standard sequemntial implementation
 *
 */
long VectorSearch(float *array, const float key, const long imin,
                  const long imax) {
  // test if array is empty
  if (imax < imin)
    // set is empty, so return value showing not found
    return KEY_NOT_FOUND;
  else {
    // calculate midpoint to cut set in half
    long imid = (imin + imax) / 2;
    // the element has been found
    if (array[imid] == key) return imid;

    long left_result = KEY_NOT_FOUND;
    long right_result = KEY_NOT_FOUND;

    // try in both sides (the array is not sorted)
    left_result = VectorSearch(array, key, imin, imid - 1);
    right_result = VectorSearch(array, key, imid + 1, imax);

    // compare results from both sides
    if (left_result > KEY_NOT_FOUND) return left_result;
    if (right_result > KEY_NOT_FOUND) return right_result;
    return KEY_NOT_FOUND;
  }
}

/*
 * Binary vector search - parallel implementation using tasks
 *
 */
long VectorSearchParallel(float *array, const float key, const long imin,
                          const long imax) {
  return KEY_NOT_FOUND;  // repklace this with your impleemntation
}

/*
 * Execute Vector searches
 *
 */
void ExecuteVectorSearches(const long size, const float key) {
  float *a = NULL;
  long Position = -1;
  AllocateMemory(&a, size);
  FillArray(a, size);

  double StartTime;
  double StopTime;

  PrintVector(a, size, "vector");
  printf("Searching for key %3.2f\n", key);

  StartTime = omp_get_wtime();
  Position = VectorSearch(a, key, 0, size);
  StopTime = omp_get_wtime();
  printf("Vector sequential search: Element found at %ld, time %fs\n", Position,
         StopTime - StartTime);

  StartTime = omp_get_wtime();
  Position = VectorSearchParallel(a, key, 0, size);
  StopTime = omp_get_wtime();
  printf("Vector parallel search  : Element found at %ld, time %fs\n", Position,
         StopTime - StartTime);
  fprintf(stdout, "\n\n");

  FreeMemory(a);
}

