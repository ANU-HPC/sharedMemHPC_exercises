# Hands-On Session #8: False Cache Line Sharing

_Objective: To explore the effect of false cache line sharing on performance._

[Instructions for accessing the remote systems](../../systems.md)

* * *

In the lectures we discussed an algorithm to compute $\pi$ using Monte Carlo techniques which generates random points within the 2d domain $(-1..1,-1..1)$ and tests whether each generated point is within a circle of radius 1 centered at the origin. The ratio of points within the circle to total points is $\frac{\pi}{4}$. The program [`pi.c`](pi.c) contains a parallel implementation of this algorithm using pthreads.

Compile the code using `make`. Run the code using

```
./pi num_threads num_points
```

The inputs are the number of threads and the number of points to select. The estimated value of $\pi$ and the time required are reported.

*   The code as provided has each thread updating a unique element in a shared array each time a point falling within the circle is selected. Run the code for some problem size and note the time required. Then modify the code to use the local variable only, copying the result back to the shared array just before each thread exits. Re-run the same problem and note the time difference.
*   (Extension) Modify the code to use PAPI to measure cache misses. Repeat the above experiment and confirm that you get many fewer cache misses when using a local variable.