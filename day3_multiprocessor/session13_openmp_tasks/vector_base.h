#ifndef VECTOR_BASE_H
#define VECTOR_BASE_H

#include <cstddef>

void AllocateMemory(float** array, const size_t size);

void FreeMemory(float* array);

void FillArray(float* array, const size_t size);

void PrintVector(float* array, const size_t size, const char* name);

#endif /* VECTOR_BASE_H */

