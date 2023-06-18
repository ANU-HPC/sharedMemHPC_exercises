# Hands-On Session #17: Lock and Barrier Performance

_Objective: To appreciate the performance issues of atomic operations in various implementations of locks and barriers._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Locks

Build the lock example code by typing:

```
cd scalable_lock
make
```

Briefly inspect the program code. The program `locktest` can evaluate five different kinds of locks:

0. simple
1. ticket
2. MCS
3. CLH
4. optimized simple

It has the usage:

`./locktest [p] [r] [t] [l]`

which gives $t$ timings (in cycles) of $r$ repetitions of acquiring lock $l$ by each of $p$ threads. It outputs $t$ lines of the form:

`[l,line_number,p,time]`

Do the following exercises.

1. The 'simple lock' is a non-scalable lock based on an atomic swap register - memory instruction (see the definition of `__xchg()` in `locks.c`). It is implemented in `simple_locks.c`. An optimization is to only try the atomic instruction when the lock becomes zero. Using the code for `simple_lock()` as a basis, in `opt_simple_lock()`, implement this optimization. Test it by typing:
    
    `./locktest [p] 1 1 4`
    
    for $p=1,2, 4,...$ (if it 'hangs', you have a deadlock bug!)
    
2. Execute the following:
    
        
          p=1 
          for l in 0 1 2 3 4; do 
            ./locktest $p 1000 5 $l
          done
        
        
    
    This will test each lock 1000 times and take five timings for one thread. Compare the median times to reduce the influence of outliers. Repeat the above for the $p = 4, 16$. What trends do you observe? Which are the most scalable? Have did your relatively simple optimization perform?

Barriers
--------

Now build the barriers example code by typing:

```
cd barriers
make
```

Briefly inspect the program code. The program `test_barriers` can evaluate three different kinds of barriers:

0. central
1. (combining) tree
2. scalable (combining tree without atomics)

It has the usage:

`./test_barriers [p] [r] [t] [b]`

which gives $t$ timings (in cycles) of $r$ repetitions of executing barrier $b$ by each of $p$ threads. It outputs $t$ lines of the form:

`[b,line_number,p,t]`

The central barrier is not scalable in that its complexity is $O(p)$, where $p$ is the number of threads.

1. Execute the following:
    
        
          p=1
          for b in 0 1 2 ; do
           ./test_barriers $p 1000 5 $b
          done
        
        
    
    This will run each barrier 1000 times and take five timings for $p$ threads. Compare the median times to reduce the influence of outliers. Repeat the above for $p = 4, 16, 32$. Which performs best? You may have noted an anomaly for $p=32$; what might have caused this?