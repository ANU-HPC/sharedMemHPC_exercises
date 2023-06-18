# Hands-On Session #10: Implementing Synchronization Constructs using pthreads

_Objectives: To understand POSIX thread creation, observe race conditions, and understand the use and implementation of bounded buffer monitors._

[Instructions for accessing the remote systems](../../systems.md)

* * *

# Race Conditions

The program [`race.c`](race.c) uses pthreads to place threads in a controlled way across multiple cores. The usage of the program is:

```
./race n p c
```

in which _p_ threads scheduled on c cores (processors) will try to increment a (shared) variable _n_ times. The default value for parameters _p_ and _c_ is one.  
Briefly inspect the code and note the use of the pthreads API for both thread creation and thread affinity.

Now compile the program by entering:

```
make race
```

Run the program with one thread, e.g.

```
./race 1000000 1
```

Now vary the number of threads from 1 to 8. Here, all threads run on a single core. Try a longer run, e.g.

```
./race 100000000 8
```

*   Do you observe any interference?
*   Why might short runs see no interference when longer ones do? (_Hint:_ on sufficiently short runs, the threads can complete within a single timeslice)

Run the program in parallel, that is, with threads running on multiple cores (say _c = p = 2,3,4_), e.g.

```
./race 1000000 2 2
```

*   How much interference do you get now?
*   You will probably see that true parallelism affects interference. You can also see the effect of increased speed by running the program under the time command, e.g.
    
        time ./race 10000000 1 1
        time ./race 10000000 4 4
    
    The `real` time is the elapsed (wall-clock) time; the `user` time is the total number of CPU-seconds that the process used in user mode; and the `sys` time is the total number of CPU-seconds that the kernel ran on behalf of the process.
    
*   Fix the race condition and verify that your program works correctly.


## Bounded Buffer Monitor

The C program bufferMon.c implements an integer bounded buffer monitor using pthreads mutex and condition synchronization. Compile and run the program using the commands

```
make bufferMon
./bufferMon <b>
```

where _b_ is the buffer size (e.g. 2, 5, 10)

*   Inspect the code and note the use of pthreads mutex and condition variable APIs. Run the program a few times for various _b_ and note the variation in the ordering of the output.
*   Using this code as a basis, implement a bounded buffer monitor in bufferSem.c using pthreads semaphores. You can compile this using make bufferSem. _Hint:_ replace the mutex and two condition variables with two semaphores, full and empty; the values of these semaphores represent the number of full and empty slots in the buffer, respectively.