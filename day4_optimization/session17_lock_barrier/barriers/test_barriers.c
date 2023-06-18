/* written by Xi Yang, 2009.
   SPARC port by Peter Strazdins, SoCS ANU, 08/09
*/

#include "barriers.h"

#ifdef SPARC
#include <sys/processor.h>
#include <sys/procset.h>
#include <sys/time.h>  /*gethrtime()*/
#include <sys/types.h> /*processor_bind()*/
#include <thread.h>    /*thr_self()*/

#include "placethreads.h" /*omp2cpuId()*/
#endif
#include <assert.h>

#define CENTRE_BAR (0)
#define COMTREE_BAR (1)
#define SCAL_BAR (2)
#ifdef EXT_SIMPE_BAR
#define SIMPLE_BAR (3)
#endif

void barrier_wait(struct thread_parameter *p) {
  switch (p->type_bar) {
    case CENTRE_BAR:
      central_barrier_wait(&p->p_bar->cen);
      break;
    case COMTREE_BAR:
      com_barrier_wait(&p->p_bar->com, p->id);
      break;
    case SCAL_BAR:
      scal_barrier_wait(&p->p_bar->scal, p->id);
      break;
    default:
      printf("Unknow type\n");
      break;
  }
}

void init_barrier(union barrier *bar, unsigned int nr_thread,
                  unsigned int type_bar) {
  switch (type_bar) {
    case CENTRE_BAR:
      init_central_barrier(&bar->cen, nr_thread);
      break;
    case COMTREE_BAR:
      init_com_barrier(&bar->com, nr_thread);
      break;
    case SCAL_BAR:
      init_scal_barrier(&bar->scal, nr_thread);
      break;
    default:
      break;
  }
}

void *thread_handler(void *par) {
  struct thread_parameter *p = (struct thread_parameter *)par;
  struct timeval start_timestamp;
  struct timeval end_timestamp;
#ifdef SPARC
  hrtime_t start_time, end_time;
#endif

  unsigned int count = 0;
  unsigned int i;
  char c = 'a' + p->id;
#ifdef DEBUG
  printf("Thread %d start\n", p->id);
#endif
  gettimeofday(&start_timestamp, NULL);
#ifdef SPARC
  start_time = gethrtime();
#endif

  for (i = 0; i < p->nr_loops; i++) {
    barrier_wait(p);
#ifdef DEBUG
    printf("Id=%d, ops\n", p->id);
#endif
    //    putchar(c);
    count++;

    barrier_wait(p);
    /*    if(p->id == 0)
          putchar('\n');*/
  }
#ifdef SPARC
  end_time = gethrtime();
#endif
  gettimeofday(&end_timestamp, NULL);
#ifdef SPARC
  p->cycles = (end_time - start_time) / 1.0E+03;
#else
  p->cycles = ((end_timestamp.tv_sec * 1000000 + end_timestamp.tv_usec) -
               (start_timestamp.tv_sec * 1000000 + start_timestamp.tv_usec));
#endif
}

static void kill_all(pthread_t *p_thr, unsigned int nr_thread) {
  unsigned int i;
  for (i = 0; i < nr_thread; i++) {
    if (p_thr[i]) {
      pthread_join(p_thr[i], NULL);
      p_thr[i] = 0;
    }
  }
}

int main(int argc, char **argv) {
  pthread_t *p_thr;
  struct thread_parameter *thread_pars;
  unsigned int nr_thread = 2;
  unsigned int count_number, loop_number, bar_type;
  unsigned int nr_loops = 2;
  union barrier bar;
  unsigned long min_cycles = 0;

  unsigned int i, j;
#ifdef SPARC
  unsigned int cores;
#else
  cpu_set_t cpu;
#endif

  if (argc < 5) {
#ifdef SPARC
    printf(
        "Usage:\n"
        "test_barriers thread_number cout_number loop_number barrier_type "
        "[cores]\n");
#else
    printf(
        "Usage:\n"
        "test_barriers thread_number cout_number loop_number barrier_type\n");
#endif
    return 1;
  }

  nr_thread = atoi(argv[1]);
  count_number = atoi(argv[2]);
  loop_number = atoi(argv[3]);
  bar_type = atoi(argv[4]);

#ifdef SPARC
  cores = (argc <= 5 ? (nr_thread + NUM_STRANDS_CORE - 1) / NUM_STRANDS_CORE
                     : atoi(argv[5]));
#endif

#ifdef DEBUG
  printf("%d threads, %d type\n", nr_thread, bar_type);
#endif

  p_thr = (pthread_t *)malloc(sizeof(pthread_t) * nr_thread);
  if (p_thr == NULL) {
    fprintf(stderr, "Error, when malloc space for p_thr\n");
    exit(1);
  }
  thread_pars = (struct thread_parameter *)malloc(
      sizeof(struct thread_parameter) * nr_thread);
  if (thread_pars == NULL) {
    fprintf(stderr, "Error, when malloc space for thread_pars\n");
    exit(1);
  }

  init_barrier(&bar, nr_thread, bar_type);
  for (j = 0; j < loop_number; j++) {
    for (i = 0; i < nr_thread; i++) {
      /*Init thread parameters*/
      thread_pars[i].p_bar = &bar;
      thread_pars[i].nr_loops = count_number;
      thread_pars[i].id = i;
      thread_pars[i].cycles = 0;
      thread_pars[i].type_bar = bar_type;
      p_thr[i] = 0;

      if (pthread_create(&p_thr[i], NULL, thread_handler,
                         (void *)(&thread_pars[i]))) {
        printf("Error: When create thread %d\n", i);
        exit(2);
      }
#ifdef SPARC
      if (processor_bind(P_LWPID, p_thr[i], omp2cpuId(i, nr_thread, cores),
                         NULL) == -1) {
        char msg[128];
        sprintf(msg, "thread %d could not bind to CPU id %d", p_thr[i],
                omp2cpuId(i, nr_thread, cores));
        perror(msg);
      }
#else
      CPU_ZERO(&cpu);
      CPU_SET(i, &cpu);
      pthread_setaffinity_np(p_thr[i], sizeof(cpu_set_t), &cpu);
#endif
    }

    kill_all(p_thr, nr_thread);
    for (min_cycles = thread_pars[0].cycles, i = 0; i < nr_thread; i++) {
      if (thread_pars[0].cycles < min_cycles)
        min_cycles = thread_pars[i].cycles;
    }
    /*    [TYPE,NR_LOOP,CYCLES]*/
    printf("[%d,%d,%d,%lu]\n", bar_type, j, nr_thread, min_cycles);
  }
}
