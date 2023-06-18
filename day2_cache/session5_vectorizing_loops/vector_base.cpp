#include "vector_base.h"

#include <malloc.h>
#include <stdio.h>

#include <cstdlib>

void AllocateMemory(float** array, const size_t size) {
  *array = (float*)memalign(16, size * sizeof(float));

}  // end of Allocate_Memory

void FreeMemory(float* array) { free(array); }  // end of Allocate_Memory

/*
 * randomize array;
 */
void FillArray(float* array, const size_t size) {
  float denominator = 1.0f / 1024.0f;

  for (size_t i = 0; i < size; i++) {
    array[i] = i;  //* denominator;
  }

}  // end of FillArray

void PrintVector(float* array, const size_t size, const char* name) {
  printf("%s: ", name);
  for (size_t i = 0; i < size; i++) {
    printf("%5.2f, ", array[i]);
  }
  printf("\n");
}

