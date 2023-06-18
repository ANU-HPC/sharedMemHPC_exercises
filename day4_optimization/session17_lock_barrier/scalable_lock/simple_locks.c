/* written by Peter Strazdins, RCCS ANU, 08/09 for comp8320 lab 4
   extended for the optimized ticket lock, 08/11
   updated by Peter Strazdins 08/15 for the CISRA Workshop
*/
#include <assert.h>

#include "locks.h"

#define LOCK_VAL 0xff

void simple_lock(spinlock_t *lock) {
  volatile unsigned long *lockptr = (unsigned long *)lock;
  unsigned long lockVal = LOCK_VAL;
  do {
    lockVal = xchg(lockptr, lockVal);
  } while (lockVal == LOCK_VAL);
}  // simple_lock()

#define TODO 0
void opt_simple_lock(spinlock_t *lock) { assert(TODO); }  // opt_simple_lock()

