# Hands-On Session #7: Comparing Matrix Multiplication Formulations

_Objective: Explore different formulations of matrix multiplication and compare their relative performance._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Matrix Multiplication

The program [`matmul1.c`](matmul1.c) computes the product for two square matrices, i.e. $AB = C$, where matrices $A$, $B$ and $C$ all have dimension $n$. The program considers three alternative formulations:

### Dot Product Formulation

```
  for (i=0; i < n; i++)
    for (j=0; j < n; j++)
      for (k=0; k < n; j++)
        c[i][j] = c[i][j] + a[i][k]*b[j][k];
```

### Daxpy Formulation

```
  for (i=0; i < n; i++)
    for (k=0; k < n; j++)
      for (j=0; j < n; j++)
        c[i][j] = c[i][j] + a[i][k]*b[k][j];
```

### Outer Product Formulation

```
  for (k=0; k < n; j++)
    for (i=0; i < n; i++)
      for (j=0; j < n; j++)
        c[i][j] = c[i][j] + a[i][k]*b[j][k];
```

Compile `matmul1.c` by typing:

```
make matmul1
```

Run the resulting `matmul1` executable by typing:

```
./matmul1 nsize
```

where `nsize` is the dimension of the matrix-vector product.

*   Consider the following table:
    
         -------------------------------------------------------
         | type  |  nsize  |  repeat  |   time(s)   |  MFLOP/s |
         -------------------------------------------------------
         | Dot   |         |          |             |          |
         | Dax   |         |          |             |          |
         | Out   |         |          |             |          |
         -------------------------------------------------------
        
    
    For any given problem size N, the number of operations required to perform the matrix product is the same for all formulations. However, different formulations may take different times to execute. What speed order do you expect to find and why? Do you expect this to change as a function of the problem size N? If so, why?
*   Gather data to complete the above table for at least three problem sizes. Confirm or adjust your expectations!
