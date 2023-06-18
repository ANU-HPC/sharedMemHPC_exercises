# Hands-On Session #9: Non-Determinism/Race Condition

_Objective: To demonstrate a race condition and fix it._

[Instructions for accessing the remote systems](../../systems.md)

* * *

A _deterministic algorithm_ will always produce the same result for a given input. A _non-deterministic algorithm_ can produce different results on different runs given the same input. Concurrent algorithms are often non-deterministic due to race conditions, in which the system's behaviour depends on the precise sequence of events. In this exercise, we will demonstrate a simple race condition.

The program [`p_sum.c`](p_sum.c) computes the sum of the first $N$ integers, which should be $N \times \frac{(N+1)}{2}$. Each thread computes the sum of a subset of the integers and then adds its partial sum to the global sum across all threads. We compute it by explicitly summing the integers in a program that uses multiple threads and sums a subset of the integers on each thread. Compile and run the program. It will only give a printout if there is an error.

*   Run the program multiple times using a while loop in the shell. For example, the following runs the code 3000 times and obtains an error on three of those runs:
    
        
          $ i=0; while [ $i -lt 3000 ] ; do  ./p_sum 10 1000; i=$(($i+1)); done
          ERROR sum computed 450800 should be 500500
          ERROR sum computed 450000 should be 500500
          ERROR sum computed 450800 should be 500500
    
*   Identify the race condition and fix the bug.
