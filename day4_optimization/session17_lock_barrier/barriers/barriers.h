#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define __USE_GNU
#include <pthread.h>
#include <sched.h>

//#define DEBUG

#define TRUE 1
#define FALSE 0

static inline volatile unsigned int fetch_add(volatile unsigned int *val,
                                              int inc) {
#ifdef SPARC
  unsigned long old, new;
  do {
    old = *val;
    new = old + inc;
    asm volatile("cas [%2], %3, %0"
                 : "=&r"(new)
                 : "0"(new), "r"(val), "r"(old)
                 : "memory");
  } while (new != old);
  return (old);
#else
  asm volatile("lock xaddl %0,%1\n" : "+Q"(inc), "+m"(*val)::"memory", "cc");
  return inc;
#endif
}

/*cen_barrier_ent*/

struct cen_barrier {
  unsigned int nr_thread;
  unsigned int counter;
  unsigned int volatile sense;
};

struct thread_parameter {
  union barrier *p_bar;
  unsigned int type_bar;
  unsigned int nr_loops;
  unsigned int id;
  unsigned long cycles;
};
void init_central_barrier(struct cen_barrier *ba, unsigned int nr_thread);
void central_barrier_wait(struct cen_barrier *ba);

/*
 *Combining tree barrier
 *         node
 *        /   \
 *      node  node
 *     /  \   /  \
 *    T   T  T    T
 *Node should be aligned on cache line bound
 */

struct com_tree_node {
  unsigned int nr_thread;
  unsigned int counter;
  unsigned int id;
  unsigned int volatile sense __attribute__((aligned(64)));
};

struct com_barrier {
  struct com_tree_node *tree;
  unsigned int nr_thread;
  unsigned int nr_node;
};

int init_com_barrier(struct com_barrier *ba, unsigned int nr_thread);
void com_barrier_wait(struct com_barrier *ba, unsigned int id);

/*Scalable_barrier*/
#define CHILD_READY (0)
#define CHILD_NOT_READY (1)
struct scal_tree_node {
  union {
    /*chk[0] is left child, chk[1] is right child*/
    unsigned char chk[2];
    unsigned short ready;
  } child_ready;
  unsigned char volatile parent_sense __attribute__((aligned(64)));
};

struct scal_barrier {
  struct scal_tree_node *tree;
  unsigned int nr_thread;
  unsigned int nr_node;
};

union barrier {
  struct cen_barrier cen;
  struct com_barrier com;
  struct scal_barrier scal;
};

int init_scal_barrier(struct scal_barrier *p_bar, unsigned int nr_thread);
void scal_barrier_wait(struct scal_barrier *p_bar, unsigned int id);
