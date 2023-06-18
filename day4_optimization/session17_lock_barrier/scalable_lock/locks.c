/* written by Xi Yang, 2009.
   SPARC port and extension for simple (non-scalable) locks
   by Peter Strazdins, RSCS ANU, 08/09, 08/11
*/

#include "locks.h"

#include <stdio.h>

static inline unsigned long __xchg(volatile void *ptr, unsigned long value) {
#ifdef SPARC
  asm volatile("swap [%2], %0"
               : "=&r"(value)
               : "0"(value), "r"(ptr)
               : "memory");
#else
  asm volatile("lock xchgq %0,%1"
               : "=r"(value)
               : "m"(*((unsigned long *)ptr)), "0"(value)
               : "memory");
#endif
  return value;
}

static inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old,
                                      unsigned long new) {
#ifdef SPARC
  asm volatile("cas [%2], %3, %0"              // instruction
               : "=&r"(new)                    // output
               : "0"(new), "r"(ptr), "r"(old)  // inputs
               : "memory");                    // side effects
  return new;
#else
  unsigned long prev;
  asm volatile("lock cmpxchgq %1,%2"
               : "=a"(prev)
               : "r"(new), "m"(*((unsigned long *)ptr)), "0"(old)
               : "memory");
  return prev;
#endif
}

#define cmpxchg(ptr, o, n)                                                   \
  ((__typeof__(*(ptr)))__cmpxchg((volatile void *)(ptr), (unsigned long)(o), \
                                 (unsigned long)(n)))
#define xchg(ptr, v) \
  ((__typeof__(*(ptr)))__xchg((volatile void *)(ptr), (unsigned long)(v)))

/*Linux ticket lock*/

void spin_lock_init(spinlock_t *lock) { lock->lock = 0; }

void spin_lock(spinlock_t *lock) {
  short inc = 0x0100;
#ifdef SPARC
  volatile unsigned long *lockptr = (unsigned long *)lock;
  unsigned long oldlockv, newlockv;

  do {  // get ticket for this thread: incremented second byte
    oldlockv = *lockptr;
    newlockv = cmpxchg(lockptr, oldlockv, oldlockv + inc);
  } while (newlockv != oldlockv);

  inc = newlockv;  // the ticket number is in the second byte
  while (((inc >> 8) & 0xff) != (inc & 0xff)) {
    // spin until lower byte matches the ticket number
    inc = (inc & 0xff00) | ((*lockptr) & 0xff);
  }
#else
  asm volatile(
      "lock xaddw %w0,%1\n"
      "1:\t"
      "cmpb %h0,%b0\n\t"
      "je 2f\n\t"
      "rep ; nop\n\t"
      "movb %1,%b0\n\t"
      "jmp 1b\n"
      "2:"
      : "+Q"(inc), "+m"(lock->lock)
      :
      : "memory", "cc");
#endif
}

void spin_unlock(spinlock_t *lock) {
#ifdef SPARC
  volatile unsigned long *lockptr = (unsigned long *)lock;
  volatile unsigned long oldlockv, newlockv;
  do {  // increment the ticket - over lower byte only!
    oldlockv = *lockptr;
    newlockv = (oldlockv & ~0xff) | ((oldlockv + 1) & 0xff);
    newlockv = cmpxchg(lockptr, oldlockv, newlockv);
  } while (newlockv != oldlockv);

#else
  asm volatile("incb %0\n\t" : "+m"(lock->lock) : : "memory", "cc");
#endif
}

#include "simple_locks.c"

void mcs_lock(mcs_lock_t *lock_p, struct qnode *node) {
  struct qnode *pre = node;
  node->next = NULL;
  pre = xchg(lock_p, pre);
  if (pre != NULL) {
    /*The lock is locked*/
    node->locked = MCS_LOCKED;
    /*X86 does not reorder Write After Write, but gcc may reorder it. Adding
      volatile instructions. mcs_unlock depends on the sequence of these two
      write instructions*/
#ifdef SPARC
    asm volatile("membar #StoreStore" ::: "memory");
#else
    //    asm volatile("sfence":::"memory");
    __asm__ __volatile__("" : : : "memory");
#endif
    pre->next = node;
    while (node->locked)
      ;
  }
}

void mcs_unlock(mcs_lock_t *lock_p, struct qnode *node) {
  if (node->next == NULL) {
    if (cmpxchg(lock_p, node, NULL) == node) {
      /*Successful*/
      return;
    }
    /*Wait the other guy complete pointers operations*/
    while (node->next == NULL)
      ;
  }
  node->next->locked = MCS_FREE;
}

void nmcs_lock(nmcslock_t *lock) {
  while (1) {
    if (NULL == cmpxchg(&lock->list, NULL, &lock->holder))
      return; /*lock->list == &lock->holder after got*/
    else {
      struct qnode instance = QNODE_UNLOCKED;
      mcs_lock_t before = lock->list;
      if (before && (before == cmpxchg(&lock->list, before, &instance))) {
        instance.locked = MCS_LOCKED;
#ifdef SPARC
        asm volatile("membar #StoreStore" ::: "memory");
#else
        asm volatile("sfence" ::: "memory");
#endif
        before->next = &instance;
        while (instance.locked == MCS_LOCKED)
          ;
        lock->holder.next = NULL;
        if (&instance != cmpxchg(&lock->list, &instance, &lock->holder)) {
          while (!instance.next)
            ;
          lock->holder.next = instance.next;
        }
        return;
      }
    }
  }
}

void nmcs_unlock(nmcslock_t *lock) {
  //  asm volatile("sfence":::"memory");
  if (&lock->holder != cmpxchg(&lock->list, &lock->holder, NULL)) {
    while (lock->holder.next == NULL)
      ;
    lock->holder.next->locked = MCS_FREE;
  }
}

extern struct lock_node *lock_nodes;
void clh_lock_init(clh_lock_t *lock, unsigned int nr_thread) {
  *lock = &lock_nodes[nr_thread].lock;
  (*lock)->locked = NOT_NEED_WAIT;
  (*lock)->next = NULL;
}

void clh_lock(clh_lock_t *lock, struct qnode **node_p) {
  struct qnode *next;
  struct qnode *node = *node_p;
  next = node;
  node->locked = NEED_WAIT;
  //  do{;}while(0);
  next = xchg(lock, next);
  node->next = next;
  while (next->locked == NEED_WAIT)
    ;
}

void clh_unlock(struct qnode **node_p) {
  struct qnode *now = (*node_p);
  *node_p = (*node_p)->next;
  //  asm volatile("sfence":::"memory");
  now->next = NULL;
  now->locked = NOT_NEED_WAIT;
}
