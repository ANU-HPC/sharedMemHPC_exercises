// simple program illustrating concurrency via `race hazards'
// for COMP2310 Lab 1, 2012.
// written by Peter Strazdins, RSCS ANU, June 2012
// compile with:
//     gcc -Wall -O -o race race.c -lpthread
// usage:
//      ./race n p c
// defaults: n=1, p=1, c=1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define __USE_GNU
#include <pthread.h>
#include <sched.h>

// increment counter ctr n times using p threads running on c cores//

static long unsigned n = 1;
static int p = 1;
static long unsigned ctr = 0;

static volatile int threadsReady = 0;  // number of threads ready to increment
static pthread_mutex_t threadsReadyLock;
static volatile int mainReady = 0;  // =1 when main thread is ready

// increment *x, with no protection against race hazards by other threads
void unsafeInc(volatile unsigned long *x) {
  unsigned long x0 = *x;
  *x = x0 + 1;
}

void *unsafeSum(void *arg) {
  long threadId = (long)arg;
  int i;

  pthread_mutex_lock(&threadsReadyLock);  // SAFE increment of threadsReady!
  threadsReady++;
  pthread_mutex_unlock(&threadsReadyLock);

#ifdef WAIT_THREADS_READY_ONLY
  while (threadsReady < p) /*spin*/;
#else
  while (mainReady == 0)
    /*spin, waiting for main program thread to be ready as well*/;
#endif

  for (i = 0; i < n / p + (threadId < n % p); i++)
    // perform the number of increments allocated for this thread
    unsafeInc(&ctr);

  return (NULL);
}  // unsafeSum()

int main(int argc, char *argv[]) {
  int i, c;
  pthread_t *threadHandle;  // (integer) handles for the created threads

  n = (argc > 1 ? atoi(argv[1]) : 1);
  p = (argc > 2 ? atoi(argv[2]) : 1);
  c = (argc > 3 ? atoi(argv[3]) : 1);
  assert(n >= 0 && p > 0 && c > 0 && c <= p);

  threadHandle = malloc(sizeof(pthread_t) * p);
  assert(threadHandle != NULL);

  pthread_mutex_init(&threadsReadyLock, NULL);

  for (i = 0; i < p; i++) {
    // create thread i; it can be run immediately
    cpu_set_t cpu;
    int thrCreateErr = pthread_create(&threadHandle[i], NULL, unsafeSum,
                                      (void *)(unsigned long)(i));
    assert(thrCreateErr == 0);

    // force thread i to run on CPU i/(p/c)
    CPU_ZERO(&cpu);
    CPU_SET(i / (p / c), &cpu);
    pthread_setaffinity_np(threadHandle[i], sizeof(cpu_set_t), &cpu);
  }

#ifndef WAIT_THREADS_READY_ONLY
  while (threadsReady < p)
    pthread_yield();  // wait until all threads have started
  mainReady = 1;      // tell threads that they can go ahead incrementing
#endif

  for (i = 0; i < p; i++) {  // wait till each thread has finished
    pthread_join(threadHandle[i], NULL);
  }
  free(threadHandle);

  printf("the result of %ld increments with %d threads in %d cores is %ld\n", n,
         p, c, ctr);

  return (0);
}  // main()
