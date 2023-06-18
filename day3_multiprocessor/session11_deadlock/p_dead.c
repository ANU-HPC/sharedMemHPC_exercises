#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id;
  int nthread;
} parm;

volatile int iflag0, iflag1;
int count;

void *cpi(void *arg) {
  parm *p = (parm *)arg;
  int myid = p->id;
  int numthread = p->nthread;
  int i, sum;
  while (count < 2) {
    if (myid == 0) {
      iflag0 = 0;
      while (iflag1 == 0)
        ;
      count++;
      for (i = 0; i < 100; i++) sum++;  // some random work
      iflag0 = 1;
    } else {
      iflag1 = 0;
      while (iflag0 == 0)
        ;
      count++;
      for (i = 0; i < 100; i++) sum++;  // some random work
      iflag1 = 1;
    }
  }
}  // cpi()

int main(int argc, char *argv[]) {
  int nthrd, npt, i, sum_check;
  parm *arg;
  pthread_t *threads;

  nthrd = 2;
  threads = (pthread_t *)malloc(nthrd * sizeof(*threads));
  count = 0;
  iflag0 = 1;
  iflag1 = 1;

  arg = (parm *)malloc(sizeof(parm) * nthrd);
  /* Spawn threads */
  for (i = 0; i < nthrd; i++) {
    arg[i].id = i;
    arg[i].nthread = nthrd;
    pthread_create(&threads[i], NULL, cpi, (void *)(arg + i));
  }
  for (i = 0; i < nthrd; i++) pthread_join(threads[i], NULL);

  printf("count %d\n", count);
  free(arg);
}  // main()
