# Hands-On Session #16: C++11 Threads and Condition Synchronization

_Objectives: To understand how to create threads in C++11, interference and deadlock, and how to implement a monitor structure._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Set Up

The Makefile for this session contains a [pattern rule](https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html) to build all the example codes.

For a given program `x`, you can build the program using the command:

```
make x
```

Try this with the program `race.cpp`, i.e. `make race`. Note the use of the compiler flags, especially `-pthread`. These are needed!

## Creating Threads

1. Inspect, build and run the program [`hello.cpp`](hello.cpp). You will notice the printed id is very long: and it gets longer each time you run it!
2. Do the same for [`hello1.cpp`](hello1.cpp). You will notice that using your own logical thread ids is much more usable. Run this a few times. You should notice that the order of the print statement varies, but we always get five unique ids from 0 to 4 (as expected).
3. Now try this for [`hello2.cpp`](hello2.cpp), which tries to do the same with the lambda function. What do you notice now? Why does this happen? Fix the lambda function so that it behaves like `hello1`.cpp.

## Race Conditions

1. Inspect, build and run for a number of times the [`race.cpp`](race.cpp) program. Do you observe any interference (causing the actual count to be less than the number of attempted increments)? How can this be fixed?
    
    If you have trouble seeing any interference, try: ``for i in `seq 0 9` ; do ./race ; done``
    

## Monitors and Deadlock

1. Inspect the program [`philosophers.cpp`](philosophers.cpp); note how the monitor (array of `Fork`) is implemented. Compile and run the program. Verify that deadlock occurs.
    
2. Make a copy of the program and modify the `philosopher()` function so that the odd-numbered philosophers pick up their forks in the opposite order. Check that this does not deadlock.
    
3. Make another copy of the original program and add a `Butler` monitor class to it. The butler will prevent more than four philosophers from sitting down at any time. It will contain a `sitdown()` function and an `arise()` function, which maintains the count of seated philosophers. The `sitdown()` function will use _condition synchronization_ to make a philosopher wait while four others are already at the table.
    
    The common butler object is passed down to `philosopher()`, which must call its `sitdown()` function before picking up the first fork and the `arise()` function after both forks have been put down. The corresponding print statements should be moved inside the `Butler` functions (after any `wait()` calls). This will ensure that the lines of output are in the order they actually occurred. Check that this version of the program does not deadlock.
    
4.  Inspect [`bufferMon.cpp`](bufferMon.cpp) and [`bufferSem.cpp`](bufferSem.cpp). These contain the monitor and semaphore-based versions of the C++ bounded buffer. Check that they work as expected.