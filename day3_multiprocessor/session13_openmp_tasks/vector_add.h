#ifndef VECTOR_ADD_H
#define VECTOR_ADD_H

#include <cstddef>

//#define PERFORMANCE

//  Vector add - standard implementation
void VectorAdd(const float* __restrict__ a, const float* __restrict__ b,
               float* __restrict__ c, const size_t size, const size_t nrpt);

void VectorAddTask(const float* __restrict__ a, const float* __restrict__ b,
                   float* __restrict__ c, const size_t size, const size_t nrpt);

// Execute Vector adds and compare run time
void ExecuteVectorAdds(const size_t size, const size_t nrpt);

#endif /* VECTOR_ADD_H */

