
# Hands-On Session 19:  Putting it All Together

_Objectives: To apply the tools and techniques learned in the previous sessions to the parallel implementation and optimization of a non-trivial linear algebra code._ 

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Cholesky Factorization

The program [`cholesky.c`](./cholesky.c) contains a sequential implementation of a [right-looking blocked Cholesky factorization](http://www.cs.utexas.edu/users/flame/Notes/NotesOnChol.pdf). In other words, given a [symmetric positive definite matrix](https://nhigham.com/2020/07/21/what-is-a-symmetric-positive-definite-matrix/) $A$, the function `cholesky` computes the lower-triangular positive matrix $L$ such that $LL^T = A$.

The program can be built using `make` and invoked using the following command:

```
./cholesky <matrix_dimension>
```

The program generates a random $n \times n$ matrix $A$ and then performs a Cholesky factorization to determine $L$. Finally, it checks the resulting factorization by computing and printing the maximum error for any element of the reconstructed matrix; that is, given $A' = LL^T$, the error is $\max(A'\_{ij} - A\_{ij})$.

After the matrix $A$ is initialized, it is copied to another matrix $L$, and the upper triangle of $L$ is set to zero. $L$ is then divided into square blocks of size `BLOCK_SIZE`, and the Cholesky factorization is performed using four basic block operations. Each of these operations corresponds to one of the [Basic Linear Algebra Subprograms](https://netlib.org/blas/), although each is specialized in some way (e.g. through hard-coded choices for certain parameters in the BLAS):

- `block_cholesky`: a non-recursive operation that overwrites a single diagonal block of the matrix with its Cholesky factorization (see [DPOTRF](https://netlib.org/lapack/explore-html/d1/d7a/group__double_p_ocomputational_ga2f55f604a6003d03b5cd4a0adcfb74d6.html));
- `block_triangular_solve`: solves the linear system $X A^T = B$ and overwrites the matrix block $B$ with the computed value of $X$ (see [BLAS DTRSM](https://www.netlib.org/lapack/explore-html/d1/d54/group__double__blas__level3_ga6a0a7704f4a747562c1bd9487e89795c.html));
- `block_symmetric_rank_k_update`: given matrix blocks $A$ and $C$, performs the update $C -= A * A^T$ (see [BLAS DSYRK](https://www.netlib.org/lapack/explore-html/d1/d54/group__double__blas__level3_gae0ba56279ae3fa27c75fefbc4cc73ddf.html)); and
- `block_sub_matrix_mul`: given matrix blocks $A$, $B$, and $C$, performs the update $C -= A * B^T$ (see [BLAS DGEMM](https://www.netlib.org/lapack/explore-html/d1/d54/group__double__blas__level3_gaeda3cbd99c8fb834a60a6412878226e1.html)).


Using the techniques you have practised in previous sessions, parallelize the code for execution on a single node of the Gadi system, and optimize its performance as far as possible.
