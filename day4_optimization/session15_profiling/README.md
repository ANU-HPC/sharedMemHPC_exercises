# Hands-On Session #15: Profiling Tools: perf & VTune

_Objective: To gain familiarity using the perf & VTune profiling tools._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## _perf : A Linux System-wide Profiling Tool_

_Credits: The perf lab is derived from various sources [Perf Wiki](https://perf.wiki.kernel.org/index.php/Tutorial); [Brenden Greg's Perf page](http://www.brendangregg.com/perf.html); [Paul Drongowski's Perf tutorial](http://sandsoftwaresound.net/perf/perf-tutorial-hot-spots/) and [Perf for user-space program analysis](https://events.static.linuxfound.org/sites/events/files/lcjp13_takata.pdf)._

### Introduction to `perf`

The `perf` tool offers a rich set of commands to collect and analyze performance and trace data.

*   Obtain the list of options for perf stat (`perf stat -h`)
*   The perf tool supports a list of measurable events. The tool and underlying kernel interface can measure events coming from different sources.
*   Obtain the list of perf events (`perf list`).
*   What is the difference between a `[Software Event]`, `[Hardware Event]` and a `[Hardware Cache Event]`? [Hint](http://web.eece.maine.edu/~vweaver/projects/perf_events/perf_event_open.html).
*   Have a look at the man page for perf stat. What do the `-r`, `-B` and `-d` options do?

*   Copy 512MB of data from `/dev/zero` into `/dev/null` and run this 10 times, using the following command:

        perf stat -d -r 10 -B dd if=/dev/zero of=/dev/null count=1000000

    Present and explain your findings. Here is a sample result:
        
         Performance counter stats for 'dd if=/dev/zero of=/dev/null count=1000000' (10 runs):
        
                424.794810      task-clock (msec)         #    0.999 CPUs utilized            ( +-  0.73% )
                         0      context-switches          #    0.001 K/sec                    ( +- 40.82% )
                         0      cpu-migrations            #    0.000 K/sec                  
                       221      page-faults               #    0.520 K/sec                    ( +-  0.09% )
             1,303,605,906      cycles                    #    3.069 GHz                      ( +-  0.77% )  (74.95%)
             2,090,641,431      instructions              #    1.60  insns per cycle        
                                                          #    0.22  stalled cycles per insn  ( +-  0.03% )  (75.08%)
               391,782,173      branches                  #  922.286 M/sec                    ( +-  0.03% )  (75.09%)
                 4,133,966      branch-misses             #    1.06% of all branches          ( +-  9.74% )  (87.56%)
               563,273,133      L1-dcache-loads           # 1325.989 M/sec                    ( +-  0.01% )  (87.14%)
                   169,702      L1-dcache-load-misses     #    0.03% of all L1-dcache hits    ( +-  1.76% )  (62.37%)
           <not supported>      LLC-loads                
           <not supported>      LLC-load-misses          
        
               0.425370876 seconds time elapsed                                          ( +-  0.73% )
        
    What does "insns per cycle" or IPC refer to? What is considered good IPC?

### Further exercises with perf

*   On your logged in session run `perf top -a --stdio`. What are you seeing?
*   Have a look at the manual page for `perf-annotate`.
    *   Add in the intel compiler paths
        
        	module load intel-compiler
            
        
    *   Compile the following code using `icc -g ./test.c`:
        
        	int main(void) {
        	  int i;
                  for (i=0; i<1000000000; i++);
                  return 0;
                }
              
        
    *   Record and annotate the above program using `perf record ./a.out` and `perf annotate -s main`
*   Have a look at the man page for perf-report. Extract the first 20 lines from the run. 

        perf report | cat -n | head -20

*   Have a look at the man-page for perf-diff.
    *   Record a perf session for `ls -l /etc` and `ls -ls /boot`
    *   Can you produce a difference listing between the two sessions?

### Profiling with perf - single threaded and multi-threaded

*   From [HandsOn Session 14](../session14_numa/), run the `matmultest` program under `perf stat` for matrix sizes 500x500, 1000x1000 and 8182x4. What differences do you note?
*   Fron [HandsOn Session 12](../../day3_multiprocessor/session12_openmp/), run your parallelized `ompexample5` program with 10000000 segments and 1 to 8 threads. Run the latter a few times. What do you notice? (you should at least notice clear differences in the CPUs utilized).


## VTune

_Credits: The VTune lab is derived from [VTune Amplifier XE Generics](http://indico.cern.ch/event/403113/contributions/1847261/attachments/1124420/1604775/08_IntelR_VTuneTM_Amplifier_XE_Generics.pdf) and the [Intel VTune tutorials](https://www.intel.com/content/www/us/en/developer/articles/training/vtune-profiler-tutorials.html)._

The general principle for profiling an application is to build it with the highest optimization levels, run a general exploratory analysis. Using results from the analysis, firstly, understand the metric being presented; secondly, identify any hardware issues and then iteratively fix this.

### Initial Setup

*   Log onto gadi using X11 forwarding to get the GUI working, i.e. use the following command from your desktop:
    
        ssh -XY userName@gadi.nci.org.au

*   Once logged in, you now need to setup your environment for vtune:
    
        module load intel-vtune 
    
### General exploration of applications using VTune

_Tutorial to follow:_ [Finding Hotspots - C++ Sample Code](https://www.intel.com/content/www/us/en/docs/vtune-profiler/tutorial-common-bottlenecks-linux/2020/overview.html). This uses the matrix application (loaded automatically).

*   Launch VTune using the following command:
    
        vtune-gui &
    
*   Now follow the tutorial. Note that the code for this tutorial is in the `~/intel/vtune/sample/matrix` folder.
