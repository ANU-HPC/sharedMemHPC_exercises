# Hands-On Session #3: Pipelining

_Objectives: To observe compiler-generated loop unrolling in the assembly code and to understand aliasing and its implications._

[Instructions for accessing the remote systems](../../systems.md)

* * *

## Assembly, Aliasing and Dependency

The file [`vadd.c`](vadd.c) contains the following code that sums together two vectors:

```c
void vadd(int n, double * __restrict__ a, double * __restrict__ b) {
    int k;
    for (k = 0; k < n; k++){
        a[k] = a[k] + b[k];
    }
}
```

 The keyword [`__restrict__`](https://cellperformance.beyond3d.com/articles/2006/05/demystifying-the-restrict-keyword.html) instructs the compiler to assume no [pointer](https://www.w3schools.com/c/c_pointers.php) [aliasing](https://developers.redhat.com/blog/2020/06/02/the-joys-and-perils-of-c-and-c-aliasing-part-1#) between these two parameters. This enables to generate more optimized code; however, it can lead to an error when `a` and `b` alias (the arrays are overlapped, i.e. `&a[i] == &b[j]`);

### Peak Performance

To execute an iteration of the loop, we load two values from memory, add them together and save the result to memory. This loop can achieve a peak performance of 1 loop iteration every 4 clock cycles assuming loop unrolling and instruction rearrangement.

### Generating Assembly Code from C

We are going to have a look at x86\_64 and ARM [assembly](https://godbolt.org). We generate this using the `-S` compiler flag. Use the following command on both an Intel and an ARM system:

`gcc -O2 -funroll-loops -S vadd.c`

or

`make vadd.s`

The assembly is placed in a file with a `.s` suffix, i.e. `vadd.s`.

### Compiler options

Note the `-funroll-loops` command tells the compiler to unroll the inner loop.

### Understanding x86 Assembly

Inspect the assembly. The entry for the main part of the for loop should be indicated by references to the label `.L3`, with the head of the loop (executed on the 2nd iteration) above at label `.L4`. Above, you can see the loop precondition. You don't need to know a huge amount of x86 assembly. Note x86 is based on the CISC architecture. Thus, arithmetic instructions can work not only with registers but also with memory. Here are some basic instructions:

```
movsd  - move 8B from memory to a register or from a register to memory
addsd  - floating point add
addq   - integer add
leaq   - load effective address - usually amounts to an integer add
cmpq   - integer comparison 
jne    - jump if not equal
```

Registers are divided into groups

```
xmm0 - xmm15    - floating point registers (up to 128b)
rax           - 64b register (64b version of x86 accumulator)
rdx           - 64b register (64b version of x86 data register)
rsi           - 64b register (64b version of x86 source index register) 
```

Look at an example of the unrolled loop, i.e. from: `gcc -O2 -funroll-loops -S vadd.c`

```assembly
movsd   (%rsi,%rax,8), %xmm0  //rax is the loop counter (k). rsi contains the address of a; xmm0 = a[k]
addsd   (%rdx,%rax,8), %xmm0  //rdi contains the address of b. xmm0 += b[k] 
leaq    1(%rax), %rcx         //rcx = rax+1         
movsd   %xmm0, (%rsi,%rax,8)  //a[k] = xmm0
movq    %rcx, %rax            //rax=rcx, i.e. k = k+1
```

Note: You can also use the following option to produce both a source listing and the assembly in a file called vadd.lst

`gcc -c -g -Wa,-a,-ad -O2 -funroll-loops vadd.c > vadd.lst`

*   From the assembly, locate all those lines that form the main body of the loop. This is all assembly instructions that are repeated as the `.L3` loop is repeated.
*   The compiler has automatically unrolled the loop. How many times has it been unrolled? Justify your answer.

We are now going to run `vadd`. The calling program is [`runadd.c`](runadd.c). Type

```
make runadd1
```

This will produce the `runadd1` executable that can be run with:

```
./runadd1 number
```

where `number` is an integer less than 500. The program allocates an array `a`, initializes the elements to 1.0 and then calls `vadd`. The values of the array are printed before and after calling `vadd`.

*   Convince yourself that the program is working correctly, i.e. for small n work out results on paper by hand and make sure you agree with the program.

Now type

```
make runadd2
```

this will use exactly the same source code to produce the runadd2 executable. Note the differences in the Makefile (these are different for the different machines as well)

*   Run `runadd2` with input of 5, 10, 15, 20, and 25.
    
    You should find the "final a" output is correct for very small input values, but wrong for higher values. (Note: if this were a real-life application, it would appear to work fine for small cases, but give big errors for large - highlighting the need to do extensive testing on real codes!) However, the results for `runadd1` are always correct.
    
    Give a detailed explanation as to what is happening and why runadd2 gives errors.
    

Now, we will investigate the performance of both variants. Type

``` 
make runadd1_perf
make runadd2_perf

./runadd1_perf 1000 1000000
./runadd2_perf 1000 1000000
```

This will run the performance benchmark with array size of 1000. The benchmark is repeated 1000000 times.

*   You should clearly see the performance difference. Write down the Average MFLOPS of both versions and explain how this is possible.

As the ARM system does not have a working Papi installed, you can use the time command for relative performance.