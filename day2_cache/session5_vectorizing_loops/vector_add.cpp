#include "vector_add.h"

#include <emmintrin.h>
#include <stdio.h>

#include "simple_papi.h"
#include "vector_base.h"

/*
 * Vector add
 * standard implementation
 *
 */

// on the gadi system with g++ 8.2.1 with -O3,
// the compiler seems to optimize away the outer loop in VectorAdd
// Incrementing this counter prevents it, and also gets the saxpy vectorized
static int repCtr;

void VectorAdd(const float *__restrict__ a, const float *__restrict__ b,
               float *__restrict__ c, const size_t size, const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    repCtr++;  // see above
    for (size_t i = 0; i < size; i++) {
      c[i] = a[i] + b[i];
    }
  }
}  // end of VectorAdd

/*
 * Vector Add
 * Unrolled implementation
 *
 */
void VectorAddUnrolled(const float *__restrict__ a, const float *__restrict__ b,
                       float *__restrict__ c, const size_t size,
                       const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    // unrolled version of vector add
    for (size_t i = 0; i < (size / 4) * 4; i += 4) {
      c[i] = a[i] + b[i];
      c[i + 1] = a[i + 1] + b[i + 1];
      c[i + 2] = a[i + 2] + b[i + 2];
      c[i + 3] = a[i + 3] + b[i + 3];
    }

    // we have to treat these elements separately if the size of the array is
    // not divisible by 4
    for (size_t i = (size / 4) * 4; i < size; i++) {
      c[i] = a[i] + b[i];
    }

  }  // for nrpt
}  // end of VectorAdd

/*
 * Vector Add
 * SSE implementation
 *
 */
void VectorAddSSE(const float *__restrict__ a, const float *__restrict__ b,
                  float *__restrict__ c, const size_t size, const size_t nrpt) {
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    // sse version of vector add
    for (size_t i = 0; i < (size / 4) * 4; i += 4) {
      __m128 sse_a = _mm_load_ps(&a[i]);
      __m128 sse_b = _mm_load_ps(&b[i]);
      __m128 sse_c = _mm_add_ps(sse_a, sse_b);
      _mm_store_ps(&c[i], sse_c);
    }

    // we have to treat these elements separately if the size of the array is
    // not divisible by 4
    for (size_t i = (size / 4) * 4; i < size; i++) {
      c[i] = a[i] + b[i];
    }

  }  // for nrpt
}  // end of VectorAdd

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

  long long StartTime;
  long long StopTime;

  StartTime = PapiStartCounters();
  VectorAdd(a, b, c, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector Add", StartTime, StopTime);

  StartTime = PapiStartCounters();
  VectorAddUnrolled(a, b, c, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector Add Unrolled", StartTime, StopTime);

  StartTime = PapiStartCounters();
  VectorAddSSE(a, b, c, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector Add SSE", StartTime, StopTime);

  fprintf(stdout, "\n\n");

  FreeMemory(a);
  FreeMemory(b);
  FreeMemory(c);
}  // end of Vector add
