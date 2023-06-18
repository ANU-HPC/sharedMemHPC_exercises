#ifndef SIMPLE_PAPI_H
#define SIMPLE_PAPI_H

void InitPapi();

// Start PAPI counters and return time
long long PapiStartCounters();

long long PapiStopCounters();

void PrintPapiResults(const char* RoutineName, long long StartTime,
                      long long StopTime);

#endif /* SIMPLE_PAPI_H */

