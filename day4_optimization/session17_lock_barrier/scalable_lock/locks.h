/* written by Xi Yang, 2009.
   SPARC port and extension for simple (non-scalable) locks
   by Peter Strazdins, RSCS ANU, 08/09, 08/11
   updated by Peter Strazdins 08/15 for the CISRA Workshop
*/

#ifndef _LOCKS_H_
#define _LOCKS_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define __USE_GNU
#include <pthread.h>
#include <sched.h>

typedef struct {
  unsigned int lock;
} spinlock_t;
void spin_lock_init(spinlock_t *lock);
void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);

typedef struct {
  unsigned int tkt, screen;
} opttktlock_t;

void opt_ticket_init(unsigned int *tkt, unsigned int *screen);
void opt_ticket_lock(unsigned int *tkt, unsigned int *screen);
void opt_ticket_unlock(unsigned int *tkt, unsigned int *screen);

/*MCS lock*/
#define MCS_LOCKED 1
#define MCS_FREE 0

void simple_lock(spinlock_t *lock);
void opt_simple_lock(spinlock_t *lock);

struct qnode {
  struct qnode *volatile next;
  volatile unsigned int locked;
};

struct lock_node {
  struct qnode lock __attribute__((aligned(64)));
};

typedef struct qnode *mcs_lock_t;
#define MCS_LOCK_UNLOCKED (mcs_lock_t) NULL
#define QNODE_UNLOCKED \
  (struct qnode) { NULL, 0 }

void mcs_lock(mcs_lock_t *lock_p, struct qnode *node);
void mcs_unlock(mcs_lock_t *lock_p, struct qnode *node);

/*K42*/
struct nmcslock_t {
  struct qnode holder;
  mcs_lock_t list;
};

typedef struct nmcslock_t nmcslock_t;

#define NMCS_LOCK_UNLOCKED \
  (nmcslock_t) { QNODE_UNLOCKED, MCS_LOCK_UNLOCKED }
#define nmcs_lock_init(x)      \
  do {                         \
    *(x) = NMCS_LOCK_UNLOCKED; \
  } while (0)
void nmcs_lock(nmcslock_t *lock);
void nmcs_unlock(nmcslock_t *lock);

/*clh lock*/
#define NEED_WAIT 1
#define NOT_NEED_WAIT 0

typedef struct qnode *clh_lock_t;

enum { NM_THREAD = 1, NM_COUNT, NM_LOOP, NM_LOCKTYPE, NM_CRITICALTYPE };
void clh_lock_init(clh_lock_t *lock, unsigned int nr_thread);
void clh_lock(clh_lock_t *lock, struct qnode **node_p);
void clh_unlock(struct qnode **node_p);

#define SIMPLE_LOCK 0
#define TICKET_LOCK 1
#define MCS_LOCK 2
#define CLH_LOCK 3
#define OPT_SIMPLE_LOCK 4

struct {
  unsigned int count;
  pthread_mutex_t mutex;
} worker;

union spin_lock {
  spinlock_t spinlock;
  mcs_lock_t mcslock;
  clh_lock_t clhlock;
  nmcslock_t nmcslock;
  opttktlock_t opttktlock;
} __attribute__((aligned(64)));
#endif
