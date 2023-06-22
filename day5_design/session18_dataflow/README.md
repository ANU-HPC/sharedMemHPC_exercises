
# Hands-On Session 18:  Task-based Dataflow Programming Using PaRSEC

_Objectives: To understand the propagation of data in a dataflow program, and implement a tree-based data flow._ 

[Instructions for accessing the remote systems](../../systems.md)

* * *

## PaRSEC

[PaRSEC](https://github.com/ICLDisco/parsec) is a generic framework for architecture aware scheduling and management of micro-tasks on distributed many-core heterogeneous architectures. Applications are expressed as a Direct Acyclic Graph of tasks with labeled edges designating data dependencies. PaRSEC assigns computation threads to the cores, overlaps communications and computations between nodes as well as between host and accelerators (like GPUs). It achieves these features by using a dynamic, fully-distributed scheduler based on architectural features such as NUMA nodes and GPU awareness, as well as algorithmic features such as data reuse.

The file [`Ex04_ChainData.jdf`](Ex04_ChainData.jdf) contains a data flow implementation for _ring_ data transfer, among the tasks.

First, the JDF begins with a preamble in C:
```
extern "C" %{

/**
 * This example shows how to use a user data, and how to modify it.
 *    data_of()
 *    parsec_data_create()
 *    parsec_data_destroy()
 *
 * @version 4.0
 * @email parsec-users@icl.utk.edu
 *
 */

#include "parsec.h"

%}

```
In this preamble, we add the includes that define the prototype of the functions that each task will call, when executed. We also include `parsec.h` and the generic include file for tiled matrix distributions. Functions definitions, or global variables, can be added here, but caution should be used, since this code will be dumped as is at the beginning of the generated C file. 

Then, we define a set of objects that are used in the tasks below. These objects are recognized by the JDF translator, and thus they must follow a non-C syntax:

```
mydata  [ type="parsec_data_collection_t*" ]
NB      [ type="int" ]
```

The task here is defined with a single parameter, k. This parameter must have an execution space that is entirely defined by the list of global objects that were defined at the beginning of the JDF. 

```
Task(k)

k = 0 .. NB

: mydata( k )

/**
 * The task now reads data from memory at a position described by the descriptor.
 * WARNING: The direct memory access read and/or write must always be colocated
 * with the task placement.
 */
RW  A <- (k == 0)  ? mydata( k ) : A Task( k-1 )
      -> (k == NB) ? mydata( k ) : A Task( k+1 )

BODY
{
    int *Aint = (int*)A;
    int rank = 0;
    *Aint += 1;

#if defined(PARSEC_HAVE_MPI)
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
#endif  /* defined(PARSEC_HAVE_MPI) */
    printf("I am element %d in the chain computed on node %d\n", *Aint, rank );
}
END
```
The task is defined with a single parameter, `k`. This parameter must have an execution space that is entirely defined by the list of global objects that were defined in the beginning of the JDF. In this case, `k` is between 0 and `NB` (inclusive). Then, the task defines its data affinity with the line `: mydata( k )` so that the task will run on the node that holds the part of the data pointed by `mydata( k )`. Each task must define a single data as its affinity.

After the data affinity comes the data flow of the task:

```
RW  A <- (k == 0)  ? mydata( k ) : A Task( k-1 )
      -> (k == NB) ? mydata( k ) : A Task( k+1 )
```

This data flow defines a single data element, `A`, that flows into `Task(k)` with the first line, and after `A` has been processed by the task, it flows out of it, towards tasks or memory, as specified in the last line. The specifier `RW` indicates that the task reads and writes the data element. 

The first line reads as _`A` comes from the memory if `k` is equal to 0, and then it is accessed directly from memory, as provided by `mydata( k )`; otherwise, `A` comes from the data element also called `A` in the task `Task( k-1 )`_.

The last element of the task definition is its body:
```
BODY
{
    int *Aint = (int*)A;
    int rank = 0;
    *Aint += 1;

#if defined(PARSEC_HAVE_MPI)
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
#endif  /* defined(PARSEC_HAVE_MPI) */
    printf("I am element %d in the chain computed on node %d\n", *Aint, rank );
}
END
```

For this session, PaRSEC has already been installed in `/scratch/c25/`. Load the required modules using the following command:
```
source /scratch/c25/session21_dataflow/module.sh
```

You can build and run the program by using the following commands:

```
make Ex04_ChainData
./Ex04_ChainData
```

## Exercise

Change the dataflow in [`Ex04_ChainData.jdf`](Ex04_ChainData.jdf) from chain of tasks to a tree that spans all the tasks.
