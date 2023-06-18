# Hands-On Session #12: Shared Memory Parallel Programming with OpenMP

_Objective: To explore the core OpenMP data parallel constructs._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Background

The OpenMP Application Program Interface (API) is a portable, scalable model that gives shared-memory parallel programmers a simple and flexible interface for developing parallel applications. The OpenMP standard supports multi-platform shared-memory parallel programming in C/C++ and Fortran. It is jointly defined by a group of major computer hardware and software vendors and major parallel computing user facilities. For more information, see the [OpenMP website](http://www.openmp.org).

OpenMP consists of a set of program directives and a small number of function/subroutine calls. The function/subroutine calls are associated with the execution runtime environment, memory locking, and timing. The directives are primarily responsible for the parallelization of the code. For C/C++ code, the directives take the form of _pragmas_:

`#pragma omp`

A program written using OpenMP directives begins execution as a single process, or "master thread". A single thread executes sequentially until it encounters the first parallel construct. When this happens, a team of threads is created, and the original thread assumes the role of master. Upon completion of the parallel construct, the threads synchronize (unless specified otherwise), and only the master continues execution. Any number of parallel constructs may be specified in the program, and as a result, the program may "fork" and "join" many times.

The number of threads that are spawned may be:

*   explicitly given as part of the pragma;
*   set using one of the OpenMP function calls; or
*   predefined by an environment variable or a system setup default.

We note that the number of threads may exceed the number of physical cores (CPUs) on the machine; this is known as *over-subscription*. When over-subscription occurs, it is up to the operating system to schedule the threads as best it can among available cores. Even if the user requests a high thread count, the OpenMP runtime will generally avoid over-subscription, as it can reduce performance. This behaviour is controlled by the `OMP_DYNAMIC` environment variable (default `true`), allowing the runtime to "adjust the number of threads to use for executing parallel regions to optimize the use of system resources". Setting `OMP_DYNAMIC=false` disables this behaviour, requiring OpenMP to spawn the requested number of threads.

## OpenMP Directives

### `parallel` Regions

A _parallel region_ is a structured block of code that is to be executed in parallel by a number of threads. Each thread executes the structured block independently. _Note: it is illegal for code to branch out of a parallel region._ The basic structure of the [parallel construct](https://www.openmp.org/spec-html/5.1/openmpse14.html#x59-590002.6) is as follows:

```c
#pragma omp parallel [clause[ [,] clause] ... ]
{
    /*structured block*/
}
```

The optional `clause`s can be used to define data sharing as follows:

*   `shared(list)` specifies variables that are visible to all threads. If you specify a variable as shared, you are stating that all threads can safely share a single copy of the variable.
*   `private(list)` specifies that each thread has its own uninitialized local copy of each variable listed.
*   `firstprivate(list)` specifies that each thread has its own local copy of each variable listed, which is initialized to the value that the variable has on entry to the block.
*   `default(data-sharing-attribute)` - where for C/C++ the `data-sharing-attribute` is either `shared` or none. When you specify the default `data-sharing-attribute`, you declare the default for all variables in the code block to be shared or to have no default (none). _Note - Fortran also permits a default of `private`. This is not available in C/C++ since many of the standard libraries use global variables, and scoping these as local would give errors._

As an example, consider the following code:

```c
#include <stdio.h>
#include <omp.h>

int main(void) {
  int i = 1, j = 2;
  printf(" Initial value of i %i and j %i \n", i, j);
#pragma omp parallel default(shared) private(i)
  {
    printf(" Parallel value of i %i and j %i \n", i, j);
    i = i + 1;
  }
  printf(" Final value of i %i and j %i \n", i, j);
  return 0;
}
```

The above code is contained in file [`ompexample1.c`](./ompexample1.c). Compile it by typing:

```
make ompexample1
```

1. Run the code with one thread by typing `./ompexample1`. What values are printed out for `i` and `j`, and why?
    
2. Now run the code with 80 threads by first setting the `OMP_NUM_THREADS` variable
    
        OMP_NUM_THREADS=80 ./ompexample1
    
    What is printed out now? (Make sure you have 80 threads). Now add `OMP_DYNAMIC=true` to the command and run it again. Explain why these values are printed.
3.  Explain the effect of changing `private(i)` to `firstprivate(i)`. What happens if you change it to `lastprivate(i)`?

### The `reduction` Clause

A [reduction clause](https://www.openmp.org/spec-html/5.1/openmpsu117.html#x152-1720002.21.5) can be added to the parallel directive. This specifies that the final values of certain variables are combined using the specified operation (or intrinsic function) at the end of the parallel region. For example, consider the program [`ompexample2.c`](./ompexample2.c), which demonstrates a number of reduction operations and also shows the use of the [`omp_get_thread_num()`](https://www.openmp.org/spec-html/5.1/openmpsu123.html#x162-1950003.2.4) routine to uniquely define each thread.

4. Run the program `ompexample2.c` with four threads and make sure you understand what is happening.

Some other useful OpenMP routines are:

*   [`omp_set_num_threads(np)`](https://www.openmp.org/spec-html/5.1/openmpsu120.html#x159-1920003.2.1): sets the number of parallel threads to be used for parallel regions
*   [`omp_get_num_threads()`](https://www.openmp.org/spec-html/5.1/openmpsu121.html#x160-1930003.2.2): returns the number of threads currently executing (note this is 1 unless you are in a parallel region)
*   [`omp_get_max_threads()`](https://www.openmp.org/spec-html/5.1/openmpsu122.html#x161-1940003.2.3): gives the maximum number of threads that could be used

The above three functions are used in the program [`ompexample3.c`](./ompexample3.c).

5. Compile and run the program `ompexample3.c`. Run the program using `OMP_DYNAMIC=true ./ompexample3 50`. How many threads are created? Now re-run using `OMP_DYNAMIC=false ./ompexample3 50`. How many threads are created now?

6. The program [`ompexample4.c`](./ompexample4.c) computes the sum of all integers from 1 to `num_elem`, and creates `p` OpenMP threads. Currently, this task is performed using the following loop, using only the main thread:
    
        sum = 0;
        i = 0;
        
        while (i < nele) {
          i++;
          sum += i;
        }
    
    Parallelize this summation by using OpenMP to manually divide (this means you are not to convert this to a `for` loop and use `#pragma omp for`) up the loop operations amongst the available OpenMP threads. Your parallel code must continue to use a `while` construct.

### The Worksharing-Loop Construct (`for`)

In the above example, you parallelized a loop by manually assigning different loop indices to different threads. With `for` loops, OpenMP provides the [worksharing-loop construct](https://www.openmp.org/spec-html/5.1/openmpsu48.html#x73-730002.11.4) to do this for you. This directive is placed immediately before a for loop and automatically partitions the loop iterations across the available threads.

```c
#pragma omp for [clause[[,]clause ...]
for (...) { ... }
```

An important optional clause is the `schedule(type[,chunk])` clause. This can be used to define specifically how the iterations are divided amongst the different threads. Two distribution schemes are:

*   `(static,chunk-size)`: iterations are divided into pieces of a size specified by chunk and these chunks are then assigned to threads in a round-robin fashion.
*   `(dynamic,chunk-size)`: iterations are divided into pieces of a size specified by chunk. As each thread finishes a chunk, it dynamically obtains the next available chunk of loop indices.

7. The program [`ompexample5.c`](./ompexample5.c) computes the value of $\pi$ by numerically integrating the function $1/(1+x^2)$ between the values of 0 and 1 using the trapezoid method (the value of this integral is $\pi/4$). The code divides the domain $[0..1]$ into N trapezoids, where the area of each trapezoid is given by the average length of the two parallel sides times the width of the trapezoid. The lengths of the two parallel sides are given by the values of the function $1/(1+x^2)$ for the lower and upper value of $x$ for that domain. Parallelize this code using the `omp parallel for` directive.
8. If the `number_of_segments` used in the integration is 100, what is the relative error in the value of $\pi$? Is this error due to rounding or truncation error?

### The `barrier` Construct

In any parallel program, there will be certain points where you wish to synchronize all your threads. This is achieved by using the [barrier construct](https://www.openmp.org/spec-html/5.1/openmpsu100.html#x133-1430002.19.2).

`#pragma omp barrier`

All threads must arrive at the barrier before any thread can continue.

### The `single` Construct

Certain pieces of code you may only want to run on one thread - even though multiple threads are executing. For example, you often only want output to be printed once from one thread. This can be achieved using the [single construct](https://www.openmp.org/spec-html/5.1/openmpsu43.html#x67-670002.10.2):

```c
#pragma omp single [clause]
{
  /*structured block*/
}
```

By default, all other threads will wait at the end of the structured block until the thread executing that block has completed. You can avoid this by augmenting the single directive with a `nowait` clause.

### The `critical` Construct

In some instances, interactions between threads may lead to wrong (or runtime variable) results. This can arise because two threads are manipulating the same data objects at the same time, and the result depends on which thread happened to start first. The [critical construct](https://www.openmp.org/spec-html/5.1/openmpsu99.html#x132-1420002.19.1) ensures that a block of code is only executed by one processor at a time. Effectively this serializes portions of a parallel region.

```c
#pragma omp critical [(name)]
{
  /*structured block*/
}
```

A thread will wait at the beginning of the critical section until no other thread in the team is executing that (named) section.

### The `atomic` Construct

The [atomic construct](https://www.openmp.org/spec-html/5.1/openmpsu105.html#x138-1480002.19.7) ensures that memory locations are accessed atomically, to help avoid race conditions or reads or writes that would result in indeterminate values.

`#pragma omp atomic`

The directive refers to the line of code immediately following it. Be aware that there may be an overhead associated with the setting and unsetting of locks - so use this directive and/or critical sections only when necessary. For example, we could use the atomic construct to parallelize an inner product:

```c
#pragma omp parallel for shared(a,b,sum) private(I,tmp)
for (i = 0; i < n; i++) {
  tmp = a[i] * b[i];
  #pragma omp atomic
  sum = sum + tmp;
}
```

but the performance would be very poor!

9. Often, it is useful to have a shared counter that can be used to implement load balancing. Program [`ompexample6.c`](./ompexample6.c) is an attempt to implement a shared counter. However, it does not work correctly (try it for various values of `maxtasks`). Explain why the current version does not work correctly.
10. Fix `ompexample6.c` so it works correctly.
11. Return to `ompexample2`. Set the number of threads to 4 and run the code. Now recompile the code using `icx` instead of `gcc`. If you look in the makefile and remove the '#' from the third and fourth lines, this will activate the `icx` compiler. You will need to enter
    
        module load intel-compiler
    
    to add `icx` to your command path. Now re-run this example with `OMP_DYNAMIC=true`. How many threads are used? Why might the behaviour be different?


<!--
## Optional Material

As mentioned in lectures all OpenMP constructs incur some overhead. As an application programmer it is important to have some feeling for the size of these overheads. (Also so you can beat up different vendors so that they produce better OpemMP implementations). In a paper presented to the European workshop on OpenMP (EWOMP) in 1999 Mark Bull (from Edinburgh Parallel Computing Centre - EPCC) presented a series of benchmarks for [Measuring Synchronization and Scheduling Overheads in OpenMP](./bullewomp1999final.pdf). The results are now somewhat old and were obtained with early versions of OpenMP enabled compilers. Thus if we repeated the benchmarks today I would expect improved results, but not orders of magnitude different.

Mark Bull has since published an [updated paper](./bullewomp2001final.pdf) that augments the benchmark suite for OpenMP 2.0 and gives more recent results - but it is not necessary for you to read this paper. There are also some results on OpenMP 3.0 are presented in a paper by Mark's student [here](./taskbenchposter.pdf)).

Read Mark Bull's [first paper](./bullewomp1999final.pdf) and then answer the following questions. Note that !$OMP DO is the Fortran equivalent to C directive #pragma omp for, and PARALLEL DO means the !$OMP PARALLEL and !$OMP DO directives are combined on a single line. Otherwise the Fortran/C relations should be obvious.

12.  Download [OpenMP micro benchmarks](http://www.epcc.ed.ac.uk/research/computing/performance-characterisation-and-benchmarking/epcc-openmp-micro-benchmark-suite) and unpack and build it. You may need to add `-fopenmp` to `CFLAGS` and `LDFLAGS` in `Makefile.defs`.  
    Run `syncbench` and fill the following table (set the number of threads by `OMP_NUM_THREADS=4 ./syncbench` etc):
    
                 
          `Overhead of OpenMP constructs -------------------+-------+-------+-------+-------+-------+                     |   1T  |  2T   |  4T   |  8T   |  16T  | -------------------+-------+-------+-------+-------+-------+  PARALLEL FOR time  |       |       |       |       |       | -------------------+-------+-------+-------+-------+-------+  Barrier   time     |       |       |       |       |       | -------------------+-------+-------+-------+-------+-------+  Critical  time     |       |       |       |       |       | -------------------+-------+-------+-------+-------+-------+  Atomic    time     |       |       |       |       |       | -------------------+-------+-------+-------+-------+-------+  Reduction time     |       |       |       |       |       | -------------------+-------+-------+-------+-------+-------+`
        
    
    Compare these results with the paper.
13. Run schedbench and fill the following table:
    
        
        Overhead of OpenMP for loop scheduling
        -------------------+-------+-------+-------+-------+-------+ 
                           |   1T  |  2T   |  4T   |  8T   |  16T  |
        -------------------+-------+-------+-------+-------+-------+ 
        STATIC     1 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        STATIC     8 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        STATIC   128 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        DYNAMIC    1 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        DYNAMIC    8 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        DYNAMIC  128 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        GUIDED     1 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        GUIDED     8 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        GUIDED   128 time  |       |       |       |       |       |
        -------------------+-------+-------+-------+-------+-------+ 
        
    
    Compare these results with the paper.

In the following loop, a, b and c are all of type double:

```c
for (i=0; i < N; i++) { 
  a[i] = a[i] + b[i]*c; 
} 
```

There are no dependencies and the loop can in principle be run in parallel.

14. Under what circumstances (values of N and numbers of threads) would it be advisable to parallelize the above loop on one of the NCI systems?
15. If you inserted the #pragma omp parallel for directive, what sort of scheduling would be advisable to use?

* * *
-->
