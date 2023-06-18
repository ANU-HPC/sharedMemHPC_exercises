/* written by Xi Yang, 2009.
   SPARC port and extension for simple (non-scalable) locks
   by Peter Strazdins, RSCS ANU, 08/09, 08/11
   updated by Peter Strazdins 08/15 for the CISRA Workshop
*/

#include "locks.h"

#ifdef SPARC
#include <sys/processor.h>
#include <sys/procset.h>
#include <sys/time.h>  /*gethrtime()*/
#include <sys/types.h> /*processor_bind()*/
#include <thread.h>    /*thr_self()*/

#include "placethreads.h" /*omp2cpuId()*/
#endif
#include <assert.h>

//#define HELLO_DEBUG 1
struct timeval start_timestamp;
struct timeval end_timestamp;

#ifdef SPARC
hrtime_t start_time, end_time;
#endif

unsigned int nm_type;
volatile unsigned int flag = 1;
volatile unsigned int stop_flag = 0;
unsigned int count_number = 0;

/*Global counter*/
struct global_counter {
  union spin_lock lock;
  volatile unsigned int counter __attribute__((aligned(64)));
} c1;

struct lock_node *lock_nodes;

void init_counter(unsigned int nr_thread) {
  c1.counter = 0;
  flag = 1;
  stop_flag = 0;
  worker.count = 0;

  switch (nm_type) {
    case TICKET_LOCK:
      spin_lock_init(&c1.lock.spinlock);
      break;
    case MCS_LOCK:
      c1.lock.mcslock = NULL;
      break;
    case CLH_LOCK:
      clh_lock_init(&c1.lock.clhlock, nr_thread);
      break;
    case SIMPLE_LOCK:
    case OPT_SIMPLE_LOCK:
      c1.lock.spinlock.lock = 0;
      break;
    default:
      assert(0);
  }
}

void lock(struct qnode **p_node) {
  struct qnode *node = *p_node;
  switch (nm_type) {
    case SIMPLE_LOCK:
      simple_lock(&c1.lock.spinlock);
      break;
    case TICKET_LOCK:
      spin_lock(&c1.lock.spinlock);
      break;
    case MCS_LOCK:
      mcs_lock(&c1.lock.mcslock, node);
      break;
    case CLH_LOCK:
      clh_lock(&c1.lock.clhlock, p_node);
      break;
    case OPT_SIMPLE_LOCK:
      opt_simple_lock(&c1.lock.spinlock);
      break;

    default:
      assert(0);
  }
}

void unlock(struct qnode **p_node) {
  struct qnode *node = *p_node;
  switch (nm_type) {
    case TICKET_LOCK:
      spin_unlock(&c1.lock.spinlock);
      break;
    case MCS_LOCK:
      mcs_unlock(&c1.lock.mcslock, node);
      break;
    case CLH_LOCK:
      clh_unlock(p_node);
      break;
    case SIMPLE_LOCK:
    case OPT_SIMPLE_LOCK:
      c1.lock.spinlock.lock = 0;
      break;
    default:
      assert(0);
  }
}

static void kill_all(pthread_t *p, unsigned int nr_thread) {
  unsigned int i;
  for (i = 0; i < nr_thread; i++) {
    if (p[i]) {
      pthread_join(p[i], NULL);
      p[i] = 0;
    }
  }
}

void *hungry_ticket_jack(void *data) {
  unsigned int i = 0;
  struct qnode *p_jack_node = &lock_nodes[(unsigned long)data].lock;

  pthread_mutex_lock(&worker.mutex);
  worker.count++;
  pthread_mutex_unlock(&worker.mutex);

#ifdef HELLO_DEBUG
  unsigned int jack_id = (unsigned int)data;
#endif

#ifdef HELLO_DEBUG
  printf("I am jack!NO %d\n", jack_id);
#endif
  while (flag)
    ;

  while (1) {
    lock(&p_jack_node);
#ifdef HELLO_DEBUG
    printf("%d got lock\n", jack_id);
#endif
    if (c1.counter > count_number) {
      if (!stop_flag) {
#ifdef SPARC
        end_time = gethrtime();
#endif
        gettimeofday(&end_timestamp, NULL);
        stop_flag = 1;
#ifdef HELLO_DEBUG
        printf("%d firt counte %d\n", jack_id, c1.counter);
#endif
        unlock(&p_jack_node);
        goto out;
      }
      /*Sorry, you are not the first one*/
      else {
        unlock(&p_jack_node);
        goto out;
      }
    }

    /*Critical section*/
    c1.counter++;

#ifdef HELLO_DEBUG
    printf("%d,relase\n", jack_id);
#endif
    unlock(&p_jack_node);
  }
out:
#ifdef HELLO_DEBUG
  printf("Jack quit.NO %d\n", jack_id);
#endif
  pthread_exit(0);
}

int main(int argc, char **argv) {
  unsigned int i, j, k;
#ifndef SPARC
  cpu_set_t cpu;
#endif
  unsigned long cycles;
  pthread_t *jack;
  unsigned int nr_thread;
  unsigned int nr_loop;
#ifdef SPARC
  unsigned int cores;
#endif

  /*NM_THREAD,NM_LOOP,NM_locktype,NM_CRITICALTYPE*/
  if (argc < 5) {
    printf(
        "Usage:\n"
#ifdef SPARC
        "locktest thread_number count_number loop_number locktype_number "
        "[cores]\n"
#else
        "locklest thread_number count_number loop_number locktype_number\n"
#endif
    );
    return 1;
  }

  pthread_mutex_init(&worker.mutex, NULL);
  nr_thread = atoi(argv[1]);
  count_number = atoi(argv[2]);
  nr_loop = atoi(argv[3]);
  nm_type = atoi(argv[4]);

#ifdef SPARC
  cores = (argc <= 5 ? (nr_thread + NUM_STRANDS_CORE - 1) / NUM_STRANDS_CORE
                     : atoi(argv[5]));
#endif

  /*Every jack have an element of lock_nodes*/
  lock_nodes =
      (struct lock_node *)malloc(sizeof(struct lock_node) * (nr_thread + 1));
  if (lock_nodes == NULL) {
    printf("Error, when malloc space for lock_nodes\n");
    exit(1);
  }

  jack = (pthread_t *)malloc(sizeof(pthread_t) * nr_thread);
  if (jack == NULL) {
    printf("Error, when malloc space for jack");
    exit(1);
  }

  /*We do the same test NM_LOOP times, try to get a stable result*/
  for (j = 0; j < nr_loop; j++) {
    init_counter(nr_thread);
    for (i = 0; i < nr_thread; i++) {
      jack[i] = 0;
      if (pthread_create(&jack[i], NULL, hungry_ticket_jack,
                         (void *)(unsigned long)(i))) {
        printf("Error: When create hungry_ticket_jack %d\n", i);
        jack[i] = 0;
        exit(2);
      }
#ifdef SPARC
      if (processor_bind(P_LWPID, jack[i], omp2cpuId(i, nr_thread, cores),
                         NULL) == -1) {
        char msg[128];
        sprintf(msg, "thread %d could not bind to CPU id %d", jack[i],
                omp2cpuId(i, nr_thread, cores));
        perror(msg);
      }
#else
      CPU_ZERO(&cpu);
      CPU_SET(i, &cpu);
      pthread_setaffinity_np(jack[i], sizeof(cpu_set_t), &cpu);
#endif
    }

    /*Waiting for all worker ready*/
    while (worker.count < i)
#ifdef SPARC
      yield();
#else
      pthread_yield();
#endif
    gettimeofday(&start_timestamp, NULL);
#ifdef SPARC
    start_time = gethrtime();
#endif
    flag = 0;
    kill_all(jack, nr_thread);
    cycles = ((end_timestamp.tv_sec * 1000000 + end_timestamp.tv_usec) -
              (start_timestamp.tv_sec * 1000000 + start_timestamp.tv_usec));
#ifdef SPARC
    cycles = (end_time - start_time) / 1.0E+3;  // microseconds
#endif
    /*[TYPE,NR_LOOPS,NR_THREAD,CYCLES]*/
    printf("[%d,%d,%d,%lu]\n", nm_type, j, nr_thread, cycles);
  }
  exit(0);
}

