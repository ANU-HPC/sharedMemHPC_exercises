#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_THREAD 20

typedef struct {
  int id;
  int nthread;
  int npoint;
} parm;

int global_sum;

void *cpi(void *arg) {
  parm *p = (parm *)arg;
  int myid = p->id;
  int numthread = p->nthread;
  int numpoint = p->npoint;
  int i, sum;
  sum = 0;
  for (i = myid; i <= numpoint; i += numthread) sum += i;
  global_sum += sum;
}

int main(int argc, char *argv[]) {
  int nthrd, npt, i, sum_check;
  parm *arg;
  pthread_t *threads;

  if (argc != 3) {
    printf("Usage: %s number_threads number_points \n", argv[0]);
    exit(1);
  }
  nthrd = atoi(argv[1]);
  npt = atoi(argv[2]);

  if ((nthrd < 1) || (nthrd > MAX_THREAD)) {
    printf("The no of threads should be between 1 and %d.\n", MAX_THREAD);
    exit(1);
  }
  threads = (pthread_t *)malloc(nthrd * sizeof(*threads));

  global_sum = 0;
  arg = (parm *)malloc(sizeof(parm) * nthrd);
  /* Spawn thread */
  for (i = 0; i < nthrd; i++) {
    arg[i].id = i;
    arg[i].nthread = nthrd;
    arg[i].npoint = npt;
    pthread_create(&threads[i], NULL, cpi, (void *)(arg + i));
  }
  for (i = 0; i < nthrd; i++) pthread_join(threads[i], NULL);

  sum_check = npt * (npt + 1) / 2;
  if (global_sum != sum_check)
    printf("ERROR sum computed %d should be %d\n", global_sum, sum_check);

  free(arg);
}
