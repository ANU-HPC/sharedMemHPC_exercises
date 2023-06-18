
# Hands-On Session #1:  Timing and Computational Scaling

_Objectives: To ensure you can access the remote machines, to measure timer overhead and resolution on these remote machines, and empirically deduce algorithm scaling from timing measurements._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## System Specifications

Confirm the specifications of the systems you log into and answer the questions below for each of the systems (_Hint: the commands `lscpu` and `uname -a` and the contents of `/proc/cpuinfo` and `/proc/meminfo` might be helpful_)

*   What is the name of the machine you are using?
*   What operating system and what version of it is running? How did you tell?
*   How many CPUs does it have, and how did you determine this?
*   What processor is present on this machine, what is its frequency (MHz), and how did you determine this information?
*   How much memory is on this machine, and how did you determine this?
*   What is the size of the lowest level cache, and how did you gain this information?

* * *

## Timer Resolution: Elapsed and CPU Time

_Objective - to measure the overhead and resolution of timers_

For benchmarking and performance measuring, knowing the resolution and overhead of your timing routine is important. We can obtain this information by making repeated calls to the timer routine. The program [walltime.c](walltime.c) does this for the C function `gettimeofday()`. Inspect the code and make sure you understand what it is doing. Then compile the code by issuing the command `make walltime`

*   Using walltime, what is your estimate for the resolution ($T_r$) and overhead ($T_c$) of `gettimeofday()` on the Intel system in seconds?

Repeat this experiment using the program [cputime.c](cputime.c), which measures the CPU time using the C function `times()`. You can compile this code by issuing the command `make cputime`

* * *

## CPU timing and Scaling Properties

_Objective - to measure CPU time and relate to computational scaling_

The program [linear.c](linear.c) sets up a set of equations of the form:  
  
$Ax = b$

where $A$ is a square matrix of dimension $n$ and $b$ is a vector of length $n$. The elements of both $A$ and $b$ are known, and the aim is to determine the elements of $x$. There are many algorithms for solving this problem, and the purpose of this exercise is to see if we can deduce the scaling of the algorithm used here based solely on timing measurements. Compile `linear.c` by typing `make linear`. Run the code by giving the problem size as a command line argument. Start with a small value of $n$, say $n=200$.

`./linear 200`

The program makes timing calls and reports the CPU and System time. We can also time the entire program using the Unix `time` command (do `man -s1 time` for details). (Why might this be useful?)

*   For at least 3 different problem sizes, complete the following table. Run each problem size at least 3 times. _Hint:_ use the following command:
    
    `n=200; (time ./linear $n; time ./linear $n; time ./linear $n ) | egrep 'CPU\+SYS'`
     
            -------------------------------------------------------------------
            |              |                 Input Dimension                  |
            |              | N1   N1   N1   | N2   N2   N2   | N3   N3   N3   |
            -------------------------------------------------------------------
            | via code     |                |                |                |
            |     CPU+SYS  |                |                |                |
            | via time     |                |                |                |
            |        real  |                |                |                |
            |        user  |                |                |                |
            |         sys  |                |                |                |
            -------------------------------------------------------------------
            
    
*   Why are the results from CPU+SYS different from the time results? (you may need to inspect the code to confirm your thoughts).
*   Using your timing data, deduce how the computational cost of linear scales with the input dimension $n$. Give the scaling as an expression of the form $O_{(nk)}$ where $k$ is an integer.

* * *
