#ifndef SAXPY_H
#define SAXPY_H

//#define PERFORMANCE

//  SAXPY - standard implementation
void SAXPY(const float* __restrict__ x, const float a, float* __restrict__ y,
           const size_t size, const size_t nrpt);

// SAXPY -unrolled version
void SAXPYUnrolled(const float* __restrict__ x, const float a,
                   float* __restrict__ y, const size_t size, const size_t nrpt);

// SAXPY- Unrolled version
void SAXPYSSE(const float* __restrict__ x, const float a, float* __restrict__ y,
              const size_t size, const size_t nrpt);

// Execute SAXPYs and compare runtime
void ExecuteSAXPYs(const size_t size, const size_t nrpt);

#endif /* SAXPY_H */

