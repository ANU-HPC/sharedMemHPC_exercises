# Hands-On Session #6: Instruction Per Cycle and Cache Misses

_Objectives: To introduce the advanced interface to PAPI and use it to measure instructions per cycle for a variety of loops and cache behaviours._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## High-Level PAPI C interface

PAPI is provided as a [module](https://opus.nci.org.au/display/Help/Environment+Modules) on the NCI system. Run the command:

```
module load papi
```

_Note: the NCI module provides PAPI version 5.7.0. The [PAPI high-level interface changed substantially in version 6](https://icl.utk.edu/files/publications/2020/icl-utk-1322-2020.pdf), so online documentation for the latest version of PAPI may not match the instructions given here._

Apart from the simplified interface introduced earlier, PAPI provides a [comprehensive High-Level API](http://icl.cs.utk.edu/projects/papi/wiki/PAPIC:High_Level). This interface allows you to define a new event subset composed of `PAPI_events` (see the output of `papi_avail`).

An example of high-level PAPI function is given in the file [`papiHighLevel.c`](papiHighLevel.c). This is similar code to that used in [Hands-On Session #2](../../day1_architecture/session2_counters). Using this interface, we first need to define a new event set and an array holding the values for selected counters. Below, you can see an event set `Events` containing two events: `PAPI_DP_OPS` and `PAPI_TOT_CYC`. The second array will maintain the values for these two counters.

```
const int NUM_EVENTS = 2;
int Events[NUM_EVENTS] = {PAPI_DP_OPS, PAPI_TOT_CYC};
long long Values[NUM_EVENTS]; 
```

The [PAPI high-level API](http://icl.cs.utk.edu/projects/papi/wiki/PAPIC:High_Level) follows a simple sequence:

1. `PAPI_start_counters(Events, NUM_EVENTS)`: take the event set and start the counters.
2. Run the code segment to be measured
3. `PAPI_stop_counters(Values, NUM_EVENTS)`: stop the counters and read their values into array values 

Build `papiHighLevel` by typing make. This version takes the same parameters as `papiSimple`. Now run `papiHighLevel` and check the output. Make sure you understand the output.

*   For one thread, the Intel Xeon processor can initiate two floating point operations (addition and multiplication) each clock cycle. Given this, how many cycles might you expect loops 1, 2 and 3 to take?
*   Complete the following table.
    
       
         --------------------------------------------------------------------------
         |                   |   Data   |    Loop 1   |    Loop 2   |    Loop 3   |
         | nsize       nrpt  | Size(MB) | instr/cycle | instr/cycle | instr/cycle |
         --------------------------------------------------------------------------
         | 10   100,000,000  |          |             |             |             |
         | 100   10,000,000  |          |             |             |             |
         | 1,000  1,000,000  |          |             |             |             |
         | 10,000   100,000  |          |             |             |             |
         | 100,000   10,000  |          |             |             |             |
         | 1,000,000  1,000  |          |             |             |             |
         | 10,000,000   100  |          |             |             |             |
         | 100,000,000   10  |          |             |             |             |
         --------------------------------------------------------------------------
    
*   Run the above instead with `papiHighLevel_O1` which is the same code compiled with **`-O1`** instead of **`-O3`**. Measure the first and last row again. Is there any difference?


## Monitoring Cache

We will now investigate L1 and L2 data cache misses and compare them with the total number of L1 and L2 data accesses. For this task, you need to decide what PAPI counters to use. Then, you have to modify the source code and rebuild it. Run the `papi_avail` command to get a list of available PAPI counters. (Note: only some of the information required in the table may be available in the list of PAPI counters. Use what is available)

*   Which performance counters would you use to determine the level 1 data cache and level 2 miss rate?
*   Complete the following table. Work only with the first loop.

```
-----------------------------------------------------------------------------------------------------------------------------------
|                  |              |          L1 data cache        |          L2 data cache        |       L3 cache (unified)      |
| nsize nrpt       | Data size[B] | misses | accesses | miss rate | misses | accesses | miss rate | misses | accesses | miss rate |
-----------------------------------------------------------------------------------------------------------------------------------
| 10   100,000,000 |              |        |          |           |        |          |           |        |          |           |
| 100   10,000,000 |              |        |          |           |        |          |           |        |          |           |
| 1,000  1,000,000 |              |        |          |           |        |          |           |        |          |           |
| 10,000   100,000 |              |        |          |           |        |          |           |        |          |           |
| 100,000   10,000 |              |        |          |           |        |          |           |        |          |           |
| 1,000,000  1,000 |              |        |          |           |        |          |           |        |          |           |
| 10,000,000   100 |              |        |          |           |        |          |           |        |          |           |
| 100,000,000   10 |              |        |          |           |        |          |           |        |          |           |
-----------------------------------------------------------------------------------------------------------------------------------
```

*   Do the L1 and L2 miss rates correlate with the MFLOPS changes recorded in the previous tables?
*   Run the above instead with `papiHighLevel_O1` which is the same code compiled with **`-O1`** instead of **`-O3`**. Measure the first and last row again. Is there any difference?
