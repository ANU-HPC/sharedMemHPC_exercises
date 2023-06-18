# Hands-On Session #4: Loop Ordering and Loop Unrolling
-------------------------------------------------------

_Objective: To investigate the effect of loop ordering and loop unrolling on performance._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Matrix-Vector Product

Perform the exercises in this session first on the Intel system. Then, if time permits, also perform them on the ARM platform to compare performance with Intel platforms.

Program [`matvec1.c`](matvec1.c) computes a matrix-vector product, $Ab = c$, for a square matrix $A$ of dimension $n$ with a vector $b$, summing the result into another vector $c$. The program has two options depending on whether we want to use the matrix $A$ or its transpose $A^T$. Furthermore, for each option, there are two different implementations, one using a dot product as the inner loop and another using a DAXPY operation as the inner loop.

### Dot Product Formulation

Normal $Ab = c$

```c
for (i=0; i < n; i++) {
  sum = 0.0;
  for (j=0; j < n; j++) {
    sum = sum + a[i][j]*b[j];
  }
  c[i] = c[i]+sum;
}
```

Transposed $A^Tb = c$

```c
for (i=0; i < n; i++) {
  sum = 0.0;
  for (j=0; j < n; j++) {
    sum = sum + a[j][i]*b[j];
  }
  c[i] = c[i]+sum;
}
```

### Daxpy Formulation

Normal $Ab = c$

```c
for (i=0; i < n; i++) {
  for (j=0; j < n; j++) {
    c[j] = c[j] + a[j][i]*b[i];
  }
}
```

Transposed $A^Tb = c$

```c
for (i=0; i < n; i++) {
  for (j=0; j < n; j++) {
    c[j] = c[j] + a[i][j]*b[i];
  }
}
```

Compile `matvec1.c` by typing:

```
make matvec1
```

This will produce the `matvec1` executable that can be run by typing:

```
./matvec1 number
```

where `number` is the dimension of the matrix-vector product.

The program loops over normal and transposed versions of the matrices and also over the different implementation versions, as follows:

```c
for (trans=0; trans < 2; trans++) {
    for (vers=0; vers < NVERSIONS; vers++) {
```

The elapsed time is measured by the lines:

```c
elp_time(&isec0, &iusec0);
for (i=0; i < repeat;i++)
  matvecmult(vers, trans, nsize, a, b, c);
elp_time(&isec1, &iusec1);
```

where the value of `repeat` is increased until the benchmark takes longer than 500usec to complete (i.e. well above the resolution of the elapsed time clock). Since the same calculation is repeated several times with different algorithms (values of `vers`), we check that the results produced by successive experiments agree (to within some threshold). If all looks OK, we print out details of the benchmark algorithm (values of `trans` and `vers`), the dimension of the problem, and the time taken for one matrix-vector product. We then move on to the next algorithm.

*   Consider the following table:
    
        ----------------------------------------------------------
        | Transpose  |  Version  |  Size  |  Time(sec) |  MFLOPS | 
        ----------------------------------------------------------
        |  Normal    |    Dot    |    N   |            |         |
        |  Normal    |    Daxpy  |    N   |            |         |
        |  Transpose |    Dot    |    N   |            |         |
        |  Transpose |    Daxpy  |    N   |            |         |
        ----------------------------------------------------------
        
    
    For any given problem size N the number of operations required to perform the matrix-vector product is the same regardless of whether the dot product or DAXPY formulation is used and whether or not the matrix is transposed. However, the four cases will not all take the same time to execute. What speed order do you expect to find and why? Do you expect this to change as a function of the problem size N? If so why?
*   Do you expect a difference between values of N which are a multiple of a power of two, and N which are not? (for example N = 1024 and N = 1025) If so, why?
*   Gather data to complete the above table for at least 3 different problem sizes. Confirm or adjust your expectations!

## Matrix-Vector Product: Loop Unrolling

Where loop iterations are independent, unrolling the loop may increase the arithmetic intensity of the inner loop, leading to better use of the superscalar hardware. In the following code snippets, the outer loop is unrolled by a factor of 2 for both formulations of the matrix-vector product:

Unrolled Normal Dot Product $Ab = c$

```c
if (n%2 == 1) {
  sum0 = 0.0;
  for (j=0; j < n; j++) {
    sum0 = sum0 + a[0][j]*b[j];
  }
  c[0] = c[0]+sum0;
}
for (i=n%2; i < n; i+=2) {
  sum1 = 0.0;
  sum2 = 0.0;
  for (j=0; j < n; j++) {
    sum1 = sum1 + a[i  ][j]*b[j];
    sum2 = sum2 + a[i+1][j]*b[j];
  }
  c[i  ] = c[i  ]+sum1;
  c[i+1] = c[i+1]+sum2;
}
```

Unrolled Normal Daxpy $Ab = c$

```c
if (n%2 == 1) {
  for (j=0; j < n; j++) {
    c[j] = c[j] + a[j][0]*b[0];
  }
}
for (i=n%2; i < n; i+=2) {
  for (j=0; j < n; j++) {
    c[j] = c[j] + a[j][i]*b[i] + a[j][i+1]*b[i+1];
  }
}
```

Program [`matvec2.c`](matvec2.c) extends `matvec1.c` with the unrolled versions of the dot product and DAXPY matrix-vector functions. Compile and run this program.

*   For a given problem size, how do you expect unrolling to affect the performance of the four possible test cases?
*   Gather data for the following table with various problem sizes. Confirm or adjust your expectations from above!
    
        --------------------------------------------------------------------------
        |   Transpose   |  Version           |  Size  |  Time(sec)  |    MFLOPS  |
        --------------------------------------------------------------------------
        |   Normal      |  Dot               |   N    |             |            |
        |   Normal      |  Daxpy             |   N    |             |            |
        |   Normal      |  Dot (unrolled)    |   N    |             |            |
        |   Normal      |  Daxpy (unrolled)  |   N    |             |            |
        |   Transpose   |  Dot               |   N    |             |            |
        |   Transpose   |  Daxpy             |   N    |             |            |
        |   Transpose   |  Dot (unrolled)    |   N    |             |            |
        |   Transpose   |  Daxpy (unrolled)  |   N    |             |            |
        --------------------------------------------------------------------------
