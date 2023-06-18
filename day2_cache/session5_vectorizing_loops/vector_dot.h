#ifndef VECTOR_DOT_H
#define VECTOR_DOT_H

//#define PERFORMANCE

//  Vector dot product - standard implementation
void VectorDot(const float* __restrict__ a, const float* __restrict__ b,
               float* __restrict__ result, const size_t size,
               const size_t nrpt);

// Vector dot product -unrolled version
void VectorDotUnrolled(const float* __restrict__ a, const float* __restrict__ b,
                       float* __restrict__ result, const size_t size,
                       const size_t nrpt);

// Vector dot product - Unrolled version
void VectorDotSSE(const float* __restrict__ a, const float* __restrict__ b,
                  float* __restrict__ result, const size_t size,
                  const size_t nrpt);

// Execute vector dot products and compare runtime
void ExecuteVectorDots(const size_t size, const size_t nrpt);

#endif /* VECTOR_DOT_H */

