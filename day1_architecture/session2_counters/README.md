
# Hands-On Session #2: Hardware Performance Counters

_Objectives: To gain experience using hardware performance counters and understand how hardware architecture can affect performance._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## The Performance Application Programming Interface: PAPI

[PAPI](https://icl.utk.edu/projects/papi/files/html_man/papi.html) is provided as a [module](https://opus.nci.org.au/display/Help/Environment+Modules) on the NCI system. Run the command:

```
module load papi
```

_Note: the NCI module provides PAPI version 5.7.0. The [PAPI high-level interface changed substantially in version 6](https://icl.utk.edu/files/publications/2020/icl-utk-1322-2020.pdf), so online documentation for the latest version of PAPI may not match the instructions given here._

Hardware performance counters are machine dependent. PAPI provides useful command line tools (`papi_avail`, `papi_cost`, `papi_mem_info`, `papi_clockres`, `papi_version`, etc.) to help determine what counters are available and to understand their output:

*   How many physical hardware counters are actually available (`papi_avail`) on either of the remote systems?
*   How many events does PAPI identify, and how many are available to count on each hardware platform (`papi_avail`)?
*   What does it mean to say that an event is derived (`papi_avail`)?
*   What is the name of the event that counts the number of CPU clock cycles (`papi_avail`)?
*   On an Intel system, are the L1, L2 and L3 caches the sizes stated earlier (`papi_mem_info`)? For BOTH the L1 and L2 caches, what are their i) sizes, ii) cache line sizes, iii) associativity? (Make sure you know what these terms mean.) Determine this information for the ARM system with L1 and L2 caches.
*   The overhead of PAPI can be measured by `papi_cost`. What is the overhead of i) starting PAPI, ii) reading a PAPI counter, and iii) resetting a PAPI counter in terms of clock cycles and seconds?
*   Is the overhead different on one of the Intel Virtual Machines compared to the native machine?
*   Is the overhead of PAPI comparable with `gettimeofday` used in the previous lab?

* * *

## Simplified PAPI C interface

PAPI provides a [high-level C application programming interface (papi.h)](https://icl.utk.edu/projects/papi/files/html_man/papi.html#3). We will use this library to measure the performance of three different loops using both the high-level and low-level PAPI interfaces.

The simplified interface is composed of this [PAPI function](https://icl.utk.edu/projects/papi/files/html_man/papi_flops.html):  

```c
#include <papi.h>
int PAPI_flops(float * rtime, float * ptime, long long * flpins, float * mflops);
 ```    

The first call to `PAPI_flops()` will initialize the PAPI High-Level interface, set up the counters to monitor `PAPI_FP_OPS` and `PAPI_TOT_CYC` events and start the counters. Subsequent calls will read the counters and return **total real time (walltime)**, **total process time**, **total floating point operations** since the start of the measurement and the **MFLOPS rate** since the latest call to `PAPI_flops()`.

We can use this simplified interface to measure the performance of three different loops in terms of MFLOPS (Million Floating Point Operations per second) on both Intel and ARM systems. The program works with two double arrays with `nsize` elements. This loop is repeated `nrpt` times.  

Look at the following code and ensure you understand what it is doing.

```c
PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

sum = 0.0;
for (int j = 0; j < nrpt; j++) {
    for (int k = 0; k < nsize; k++){
        //-- loop one --//
        sum = sum + (a[k]*b[k]);

        //-- loop two --//
        //sum = sum + a[k] * (a[k]+b[k]);

        //-- loop three --//
        //  if (a[k] > 0.5) sum = sum + (a[k]*b[k]);
        //  else            sum = sum - (a[k]*b[k]);

    }// for k
}// for j

PAPI_flops(&real_time, &proc_time, &flpins, &mflops);   
```

* * *

## Exercises

The simple interface is in the file [`papiSimple.c`](papiSimple.c). An equivalent program using the high-level interface is in [`papiHighLevel.c`](papiHighLevel.c). Build both programs by typing `make`. The programs take as command line inputs two parameters, `nsize` and `nrpt`, where `nsize` is the length of the vectors `a` and `b`.

*   Run first the simple program. What happened and why? Use the `papi_avail` command to see if the required `PAPI_FP_OPS` counter is available on this system. Check also if `PAPI_FP_INS` (also widely used) is available.
*   Inspect [`papiHighLevel.c`](papiHighLevel.c). What counter does it use instead? Run the program. Do all the operations in the loop seem to be counted? How about the floating point comparison in loop 3?
*   Use hardware performance counters to measure the performance of the three loops. Consider the following table.
    
           
             -------------------------------------------------------------------
             |                   |    Data   |  Loop 1  |   Loop 2  |  Loop 3  |
             | nsize       nrpt  |  Size(MB) |  MFLOPS  |   MFLOPS  |  MFLOPS  |
             -------------------------------------------------------------------
             | 10   100,000,000  |           |          |           |          |
             | 100   10,000,000  |           |          |           |          |
             | 1,000  1,000,000  |           |          |           |          |
             | 10,000   100,000  |           |          |           |          |
             | 100,000   10,000  |           |          |           |          |
             | 1,000,000  1,000  |           |          |           |          |
             | 10,000,000   100  |           |          |           |          |
             | 100,000,000   10  |           |          |           |          |
             -------------------------------------------------------------------
        
        
    
    *   For each loop, how do you expect the performance to vary from row to row (i.e. as nsize increases and nrpt decreases) and why?
    *   For a given row (nsize/nrpt pair), which loop do you expect to run the fastest, and why?
    *   Now run the actual experiments and complete the table for the Intel system. Do your expectations agree with your measurements? If not, can you explain?  
        
    *   For loop 3, how would you expect your result to change as a function of the value used in the threshold test (currently set to 0.5)?