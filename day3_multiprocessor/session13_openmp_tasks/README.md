# Hands-On Session #13: OpenMP Tasks

_Objective: To use OpenMP tasks to parallelize a recurisve algorithm._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## OpenMP Tasks

The concept of [tasks](https://www.openmp.org/spec-html/5.0/openmpsu46.html#x70-2000002.10.1) was added in [OpenMP 3.0](http://openmp.org/). This feature allows a wider range of applications to be parallelized, including recursive algorithms such as linked lists and tree traversals, quicksort, binary search, etc.

A task has:

*   code to execute (structured block, function, etc.);
*   a data environment (it owns its data); and
*   an assigned thread that executes the code and uses the data.

Each thread that encounters a task packages a new instance of a task (code and data) and puts it into a queue. The tasks are removed from the queue by threads sometime later. Under some conditions, the task can migrate between threads (`untied` clause) or be executed immediately.

### The `task` Construct

`#pragma omp task [clause[[,]clause] ...] structured - block`

where `clause` can be one of:

```
if (expression)  // if the expression is false, the task is executed immediately
  untied         // task may migrate between threads
  shared(list)
  private(list)
  firstprivate(list)
  default(shared | none)
```

Take a look at this simple example:

```c
#pragma omp parallel
{
  #pragma omp single private(p)
  {
    p = listhead;

    while (p) {
#pragma omp task
      process(p);
      p = next(p);
    }
  }
}
```

All the threads enter the parallel region. You can specify how many threads are wanted to execute this part of the code. Then, only a single thread enters a structured block and reads the head of the linked list. This thread traverses the list and generates a task for every element of the list. Be aware that those tasks will be executed in parallel at some point in the future. All tasks will have been finished when leaving the parallel construct (an implicit barrier).

### Fibonacci numbers

The program [`fibonacci.cpp`](./fibonacci.cpp) uses OpenMP tasks to calculate Fibonacci numbers. Compile it by typing:

```
make fibonacci
```

The program accepts two parameters: the number of threads and the Fibonacci number we wish to calculate.

1. Run the code by typing `./fibonacci 1 30`. What value is printed out? Make sure you understand what the code is doing - what is the meaning of the `#pragma omp taskwait` construct, and what happens if you remove it?
2. Now run the code with different numbers of threads and compare the execution time. Comment on the observed behaviour.
3. Verify if the code is really being executed by multiple threads by adding an appropriate function call (`printf`) into the fib function.

### Vector addition using OpenMP tasks

The program [`vector_add.cpp`](./vector_add.cpp) implements vector addition using OpenMP tasks. There are three functions in this file:

```
VectorAdd           - Standard OpenMP implementation of vector add 
VectorAddTask       - Your implementation using OpenMP tasks
ExecuteVectorAdds   - Function that runs both versions and shows the results
```

You can run this example by typing:

```
vector_add num_of_threads vector_size num_of_repetitions
```

When you've finished the implementation, run the code by typing:

```
./vector_add 1 10 1 ./vector_add 4 10 1
```

4. Implement vector addition using OpenMP tasks. Verify that you get consistent results for varying numbers of threads. _Hint:_ create a new task to perform the summation of each individual element.
5. Open the file `vector_add.h` and uncomment line 6 (`#define PERFORMANCE`).
6. Recompile the program and fill in the following table. You should be able to explain the results.
     
        Execution time of vector add
        -----------------------+---------------+---------------+ 
                            |   loop time   |    task time  |
        -----------------------+---------------+---------------+ 
        ./vector_add 1 2 2048  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 2 2 2048  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 4 2 2048  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 1 256 32  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 2 256 32  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 4 256 32  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 1 1024 8  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 2 1024 8  |               |               |
        -----------------------+---------------+---------------+
        ./vector_add 4 1024 8  |               |               |
        -----------------------+---------------+---------------+

7. From the table, you can see a slowdown when working with tasks. The problem is that we generate a huge number of tiny tasks. Open the file [`vector_add.h`](./vector_add.h) and comment out line 6 (`#define PERFORMANCE`) again - we want to modify the implementation. Now, rework your code to create a much smaller number of much bigger tasks (each task adds a sub-vector, not a single element). When it works correctly, uncomment `#define PERFORMANCE` in `vector_add.h`, recompile and fill in the table again. Explain the results.

        Execution time of vector add (tasks operating on sub-vectors)
        -----------------------+---------------+---------------+ 
                            |   loop time   |    task time  |
        -----------------------+---------------+---------------+ 
        ./vector_add 1 2 2048  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 2 2 2048  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 4 2 2048  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 1 256 32  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 2 256 32  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 4 256 32  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 1 1024 8  |               |               |
        -----------------------+---------------+---------------+ 
        ./vector_add 2 1024 8  |               |               |
        -----------------------+---------------+---------------+
        ./vector_add 4 1024 8  |               |               |
        -----------------------+---------------+---------------+
    

* * *

## Tasks Extension (Optional)

The OpenMP task paradigm is usually used to parallelize recursive algorithms. Your task in this example is to implement a parallel version of the binary search in an unsorted array. In each step, the algorithm compares the input key value with the key value of the middle element of the array. If the keys match, then a matching element has been found, so its index (position) is returned. Otherwise, the algorithm tries to look up the key in the left and the right part of the array.

The algorithm is implemented in file [`vector_search.cpp`](./vector_search.cpp). There are three functions in this file:

```
VectorSearch          - Standard sequential recursive implementation of this search algorithm
VectorSearchParallel  - Your implementation using OpenMP tasks
ExecuteVectorSearches - Function that runs both versions and shows the results
```

You can run this example by typing:

```
./vector_search num_of_threads vector_size key
```

While implementing, run the code with the following parameters to verify it:

```
./vector_search 1 10 9
./vector_search 4 10 9
```

8. Implement a parallel search algorithm using OpenMP tasks, verify it on arrays of different sizes, and check that multiple threads are used.
