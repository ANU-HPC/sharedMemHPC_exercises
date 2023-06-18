#include "saxpy.h"

#include <emmintrin.h>
#include <stdio.h>

#include "simple_papi.h"
#include "vector_base.h"

//  SAXPY - standard implementation
void SAXPY(const float *__restrict__ x, const float a, float *__restrict__ y,
           const size_t size, const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    for (size_t i = 0; i < size; i++) y[i] += a * x[i];
  }
}  // end of SAXPY

// SAXPY - unrolled version
void SAXPYUnrolled(const float *__restrict__ x, const float a,
                   float *__restrict__ y, const size_t size,
                   const size_t nrpt) {
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    for (size_t i = 0; i < (size / 4) * 4; i += 4) {
      y[i] += a * x[i];
      y[i + 1] += a * x[i + 1];
      y[i + 2] += a * x[i + 2];
      y[i + 3] += a * x[i + 3];
    }

    // we have to treat these elements separately if the size of the array is
    // not divisible by 4
    for (size_t i = (size / 4) * 4; i < size; i++) {
      y[i] += a * x[i];
    }

  }  // for nrpt
}  // end of SAXPYUnrolled

// SAXPY- Unrolled version
void SAXPYSSE(const float *__restrict__ x, const float a, float *__restrict__ y,
              const size_t size, const size_t nrpt) {
  const size_t UNROLL_FACTOR = 16;
  // repeat many times to spend reasonable amount of time here
  for (size_t rpt = 0; rpt < nrpt; rpt++) {
    ///////////////////////////////////////////////////////////////////////////////
    //              		PUT YOUR SSE CODE HERE                       //
    //                                                                           //
    ///////////////////////////////////////////////////////////////////////////////

  }  // for nrpt
}  // end of SAXPYSSE

/*
 * Execute SAXPYs and compare runtime
 *
 */
void ExecuteSAXPYs(const size_t size, const size_t nrpt) {
  float *x = NULL;
  float *y = NULL;
  float a = 0.01f;

  AllocateMemory(&x, size);
  AllocateMemory(&y, size);

  FillArray(x, size);
  FillArray(y, size);

  long long StartTime;
  long long StopTime;

#ifndef PERFORMANCE
  PrintVector(x, size, "x");
  PrintVector(y, size, "y");
#endif

  StartTime = PapiStartCounters();
  SAXPY(x, a, y, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector SAXPY ", StartTime, StopTime);

#ifndef PERFORMANCE
  PrintVector(y, size, "y_SAXPY");
#endif

  // reinitialize y
  FillArray(y, size);
  StartTime = PapiStartCounters();
  SAXPYUnrolled(x, a, y, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector SAXPY Unrolled", StartTime, StopTime);
#ifndef PERFORMANCE
  PrintVector(y, size, "y_SAXPYUnroll");
#endif

  // reinitialize y
  FillArray(y, size);
  StartTime = PapiStartCounters();
  SAXPYSSE(x, a, y, size, nrpt);
  StopTime = PapiStopCounters();
  PrintPapiResults("Vector SAXPY SSE", StartTime, StopTime);

#ifndef PERFORMANCE
  PrintVector(y, size, "y_SAXPYSSE");
#endif

  FreeMemory(x);
  FreeMemory(y);
}  // end of ExecuteSAXPYs
