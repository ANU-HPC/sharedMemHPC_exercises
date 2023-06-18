#ifndef VECTOR_ADD_H
#define VECTOR_ADD_H

#define PERFORMANCE

//  Vector add - standard implementation
void VectorAdd(const float* __restrict__ a, const float* __restrict__ b,
               float* __restrict__ c, const size_t size, const size_t nrpt);

// Vector Add - unrolled implementation
void VectorAddUnrolled(const float* __restrict__ a, const float* __restrict__ b,
                       float* __restrict__ c, const size_t size,
                       const size_t nrpt);

//  Vector Add - SSE implementation
void VectorAddSSE(const float* __restrict__ a, const float* __restrict__ b,
                  float* __restrict__ c, const size_t size, const size_t nrpt);

// Execute Vector adds and compare run time
void ExecuteVectorAdds(const size_t size, const size_t nrpt);

#endif /* VECTOR_ADD_H */

