#include "vector_dot.h"

#include <emmintrin.h>
#include <stdio.h>

#include "simple_papi.h"
#include "vector_base.h"

/*
 * Vector dot product - standard implementation
 */
void VectorDot(const float *__restrict__ a, const float *__restrict__ b,
               float *__restrict__ result, const size_t size,
               const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    float sum = 0.0f;
    for (size_t i = 0; i < size; i++) {
      sum += a[i] * b[i];
    }
    *result = sum;
  }
}  // end of VectorDotProduct

/*
 * Vector dot product - unrolled version
 */
void VectorDotUnrolled(const float *__restrict__ a, const float *__restrict__ b,
                       float *__restrict__ result, const size_t size,
                       const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    // unrolled version of vector dot product
    float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (size_t i = 0; i < (size / 4) * 4; i += 4) {
      sum[0] += a[i] * b[i];
      sum[1] += a[i + 1] * b[i + 1];
      sum[2] += a[i + 2] * b[i + 2];
      sum[3] += a[i + 3] * b[i + 3];
    }

    // we have to treat these elements separately if the size of the array is
    // not divisible by 4
    for (size_t i = (size / 4) * 4; i < size; i++) {
      sum[0] += a[i] * b[i];
    }

    // sum the partial sums together
    *result = sum[0] + sum[1] + sum[2] + sum[3];
  }
}  // end of VectorDotProduct

/*
 * Vector dot product - unrolled version
 */
void VectorDotSSE(const float *__restrict__ a, const float *__restrict__ b,
                  float *__restrict__ result, const size_t size,
                  const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    float sum[4] __attribute__((aligned(16))) = {0.0f, 0.0f, 0.0f, 0.0f};

    __m128 sse_sum = _mm_set1_ps(0.0f);

    // SSE version of vector dot product
    for (size_t i = 0; i < (size / 4) * 4; i += 4) {
      __m128 sse_a = _mm_load_ps(&a[i]);
      __m128 sse_b = _mm_load_ps(&b[i]);
      ///////////////////////////////////////////////////////////////////////////
      //									 //
      //	          	PUT THE SSE OPERATIONS HERE //
      ///////////////////////////////////////////////////////////////////////////
    }

    _mm_store_ps(sum, sse_sum);

    // we have to treat these elements separately (non-SSE) if the size of the
    // array is not divisible by 4
    for (size_t i = (size / 4) * 4; i < size; i++) {
      sum[0] += a[i] * b[i];
    }

    *result = sum[0] + sum[1] + sum[2] + sum[3];

  }  // for rpt

}  // end of VectorDotProduct

/*
 * Execute all vector dots and compare run time
 *
 */
void ExecuteVectorDots(const size_t size, const size_t nrpt) {
  float *a = NULL;
  float *b = NULL;

  AllocateMemory(&a, size);
  AllocateMemory(&b, size);

  FillArray(a, size);
  FillArray(b, size);

  long long StartTime;
  long long StopTime;

  float result1 = 0.0f, result2 = 0.0f, result3 = 0.0f;

#ifndef PERFORMANCE
  PrintVector(a, size, "a");
  PrintVector(b, size, "b");
#endif

  StartTime = PapiStartCounters();
  VectorDot(a, b, &result1, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector dot ", StartTime, StopTime);

  StartTime = PapiStartCounters();
  VectorDotUnrolled(a, b, &result2, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector dot Unrolled", StartTime, StopTime);

  StartTime = PapiStartCounters();
  VectorDotSSE(a, b, &result3, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector dot SSE", StartTime, StopTime);

#ifndef PERFORMANCE
  PrintVector(&result1, 1, "Vector dot         ");
  PrintVector(&result2, 1, "Vector dot unrolled");
  PrintVector(&result3, 1, "Vector dot SSE2    ");
#endif

  fprintf(stdout, "\n results = %f,%f,%f\n", result1, result2, result3);

  FreeMemory(a);
  FreeMemory(b);
}  // end of ExecuteVectorDots

