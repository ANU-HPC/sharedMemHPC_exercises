# Hands-On Session #11: Deadlock

_Objective: To demonstrate a simple deadlock scenario._

[Instructions for accessing the remote systems](../../systems.md)

* * *

The program [`p_dead.c`](p_dead.c) attempts to use two shared variables to enforce mutual exclusion. Compile and run the code. You should find that sometimes the code terminates correctly, whereas sometimes it deadlocks, requiring an interrupt (Ctrl-c) to abort execution.

Examine the code:

*   Understand why the program fails to terminate normally on some runs.
*   When it does terminate, the value of `count` printed is not always the same. Why is this?

You may also wish to use a debugger to inspect the state of the program at the point of deadlock. Using `gdb`, you can [attach to a running process](https://sourceware.org/gdb/onlinedocs/gdb/Attach.html) by entering 

```
attach _process_id
```

and then [inquire about the status of running threads](https://sourceware.org/gdb/onlinedocs/gdb/Threads.html#Threads) by entering 

```
info threads
```
