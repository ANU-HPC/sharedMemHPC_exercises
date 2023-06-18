# Hands-On Session #5: Vectorizing loops using SSE & NEON instructions

_Objective: To practice programming with SIMD instructions._

[Instructions for accessing the remote systems](../../systems.md)

* * *

The most direct way to use these instructions is to inline the assembly language instructions into your source code. However, this process can take time and effort. In addition, your compiler may not support inline assembly language programming. The GNU and Intel C++ Compiler enable easy implementation of these instructions through API extension sets built into the compiler. These extension sets are referred to as intrinsic functions or intrinsics.

## Intel SSE2

For this exercise, you may need to refer to the [Intel SIMD Intrinsics Guide](https://software.intel.com/sites/landingpage/IntrinsicsGuide/) or the complete [Intel 64 Instruction Set Reference](http://www.intel.com.au/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf).

The `emmintrin.h` header file contains the declarations for the SSE2 intrinsics. This header file defines all the instructions and data types in SSE2.

SSE2 intrinsics can be used with the following data types:

*   `__m128` - packed FP datatype containing 4 floats
*   `__m128d` - packed FP datatype containing 2 doubles
*   `__m128i` - packed FX datatype containing 4 ints, or 8 shorts or 16 bytes

### Naming and usage syntax for SSE2 instructions

Most intrinsic names use the following notational convention:  

```
_mm_<intrin_op>_<suffix>
```

*   `<intrin_op>` Indicates the basic operation of the intrinsic; for example, `add` for addition and `sub` for subtraction
*   `<suffix>` Denotes the type of data the instruction operates on. The first one or two letters of each suffix denote whether the data is packed (`p`), extended packed (`ep`) (this relates to the length of the SIMD register), or scalar (`s`). The remaining letters and numbers denote the type, with notation as follows:

```
   s    - single-precision floating point
   d    - double-precision floating point
   i128 - signed 128-bit integer
   i64  - signed 64-bit integer
   u64  - unsigned 64-bit integer
   i32  - signed 32-bit integer
   u32  - unsigned 32-bit integer
   i16  - signed 16-bit integer
   u16  - unsigned 16-bit integer
   i8   - signed 8-bit integer
   u8   - unsigned 8-bit integer
```

### Most frequently used SSE2 intrinsics

#### Load operations:

```assembly
  __m128 _mm_load1_ps(float * p)       // Load one SP FP value into all four words
  __m128 _mm_load_ps (float * p)       // Load four SP FP values, the address must be 16-byte-aligned 
```

#### Set operations:

```assembly
  __m128 _mm_set1_ps(float w)           // Set the four SP FP values to w 
  __m128 _mm_set_ps(float z, float y,   // Set the four SP FP values to the four inputs
                   float x, float w ) 
```

#### Store operations:

```assembly
  void _mm_store1_ps(float * p, __m128 a) // Stores the lower SP FP value across four words.  
  void _mm_store_ps(float *p, __m128 a)   // Stores the four SP FP values. The address must be 16-byte-aligned
```

#### Arithmetic operations:

```assembly
  __m128 _mm_add_ss(__m128 a, __m128 b)  // Add the lower single-precision, floating-point (SP FP) values of a and b
                                         // The upper 3 SP FP values are passed through from a
  __m128 _mm_add_ps(__m128 a, __m128 b)  // Add two registers element by element
```

### SSE vector add example

Our first SSE example will focus on vector addition implemented in SSE. You can find the implementation of three versions of vector addition in the file [`vector_add.cpp`](vector_add.cpp). There are four functions in this file:

    VectorAdd           - Standard implementation of vector addition
    VectorAddUnrolled   - Unrolled version of vector addition
    VectorAddSSE        - SSE implementation of vector addition
    ExecuteVectorAdds   - Function that runs all versions and shows the results

1. Make the vector addition program by typing
    
    `make vector_add 2> make_output`
    
    The output make_output should contain a very long report from the compiler. It provides you with details about the auto-vectorization process used in Standard and Unrolled implementation. Of course, the compiler didn't need to vectorize the code that was already hand-vectorized using SSE. Go through the output and cross-reference each reported line of code to the source to determine which loops were vectorized. _Hint:_ search for `loop vectorized`.
2. You can run the code by typing
    
    `./vector_add vector_size_kb number_of_repetitions`
    
    The parameter `vector_size_kb` is multiplied by 1024 in order to work with large arrays. Look at [http://icl.cs.utk.edu/projects/papi/wiki/PAPITopics:SandyFlops](http://icl.cs.utk.edu/projects/papi/wiki/PAPITopics:SandyFlops) and change the definition of the PAPI counter in `simple_papi.cpp` to count the number of vector operations (see section on PAPI Preset Definitions). Which counter gives the most consistent number of FLOPs for the 3 codes? Use this counter from now on.
_Note: If you inspect the code, you will notice that only operations on vectors of sizes 4 or greater are vectorized. In general, when vectorization does occur, `PAPI_SP_OPS` might record 0 and so `PAPI_VEC_SP` should be used. Due to a limitation of PAPI, only either one of `SP_OPS` or `VEC_SP` can be counted simultaneously_  
  
6. Complete the following table.
    
                 Execution time and MFLOPS of vector add
          ----------------------------------------------------------------
          |                    |        | exec. time(ms) |    MFLOPS     |
          |--------------------|--------|----------------|---------------|
          |vector_add 2 8192   | Add    |                |               |
          |                    | Unroll |                |               |
          |                    | SSE    |                |               |
          |--------------------|--------|----------------|---------------|
          |vector_add 256 1024 | Add    |                |               |
          |                    | Unroll |                |               |
          |                    | SSE    |                |               |
          |--------------------|--------|----------------|---------------|
          |vector_add 2048 64  | Add    |                |               |
          |                    | Unroll |                |               |
          |                    | SSE    |                |               |
          ----------------------------------------------------------------
    

### SSE vector dot product

The next example deals with an SSE implementation of the vector dot product. You can find the implementation of three versions of vector dot product in the file [`vector_dot.cpp`](vector_dot.cpp). There are four functions in this file:

    VectorDot           - Standard implementation of vector dot product
    VectorDotUnrolled   - Unrolled version of vector dot product
    VectorDotSSE        - SSE implementation of vector dot product
    ExecuteVectorDots   - Function that runs all versions and shows the results

1. Make the vector dot product program by typing
    
    `make vector_dot 2> make_output`
    
    How many loops was the compiler able to vectorize?
2. Inspect the source code in `vector_dot.cpp`. When you reach `VectorDotSSE`, you will see that something is missing. To complete the code, you have to add two SSE instructions here. Make these changes, then verify the code by typing:
    
    `./vector_dot 10 1`
    
3. (OPTIONAL). Run the code with input a vector length of 372 and 1 repeat. Look carefully at the three computed values for the vector dot product. You may find the results differ; for example, 17090484 for the standard code but 17090486 for the unrolled and SSE code. Which value is correct? Why is your program reporting "wrong" answers in some circumstances? Repeat this experiment for slightly different values of vector length, e.g. 371 and 373.
4. When the code is running correctly, we want to measure its performance. Open the header file `vector_dot.h` and uncomment line 11 (`#define PERFORMANCE`) and rebuild the program. Complete the following table:
    
           
                 Execution time and MFLOPS of vector dot product
          ----------------------------------------------------------------
          |                    |        | exec. time(ms) |    MFLOPS     |
          |--------------------|--------|----------------|---------------|
          |vector_dot 2 8192   | Dot    |                |               |
          |                    | Unroll |                |               |
          |                    | SSE    |                |               |
          |--------------------|--------|----------------|---------------|
          |vector_dot 256 1024 | Dot    |                |               |
          |                    | Unroll |                |               |
          |                    | SSE    |                |               |
          |--------------------|--------|----------------|---------------|
          |vector_dot 2048 64  | Dot    |                |               |
          |                    | Unroll |                |               |
          |                    | SSE    |                |               |
          ----------------------------------------------------------------
    
    The performance should be much higher here. Explain why.

### SSE implementation of a SAXPY loop

The last example focuses on a SAXPY loop implementation (the same as DAXPY but with single-precision floating point numbers). You can find the implementation of three versions of SAXPY in file `saxpy.cpp`. There are four functions in this file:
   
    SAXPY           - Standard implementation of vector SAXPY
    SAXPYUnrolled   - Unrolled version of vector SAXPY
    SAXPYSSE        - SSE implementation of vector SAXPY
    ExecuteSAXPYs   - Function that runs all versions and shows the results

1. Based on two codes provided in the file [`saxpy.cpp`](saxpy.cpp), create an SSE implementation. You can make the SAXPY executable by typing
    
    `make vector_saxpy 2> make_output`
    
2. When you are sure the code produces the right answer, measure the performance of the code. Do not forget to uncomment line 11 (`#define PERFORMANCE`) in `saxpy.h`. Complete the table below, and comment on the differences.

             Execution time and MFLOPS of vector saxpy
     ------------------------------------------------------------------
     |                      |        | exec. time(ms) |    MFLOPS     |
     |----------------------|--------|----------------|---------------|
     |vector_saxpy 2 8192   | Saxpy  |                |               |
     |                      | Unroll |                |               |
     |                      | SSE    |                |               |
     |----------------------|--------|----------------|---------------|
     |vector_saxpy 256 1024 | Saxpy  |                |               |
     |                      | Unroll |                |               |
     |                      | SSE    |                |               |
     |----------------------|--------|----------------|---------------|
     |vector_saxpy 2048 64  | Saxpy  |                |               |
     |                      | Unroll |                |               |
     |                      | SSE    |                |               |
     ------------------------------------------------------------------

4. A possible way to improve performance further is by unrolling the SSE loop, possibly in conjunction with software pipelining, to overlap the data dependencies. Later, if you have time, try this (and see for what array sizes, if any, performance improves).

## ARM NEON

The ARM architecture uses [NEON SIMD instructions](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0472m/chr1359125038862.html). The `arm_neon.h` header file contains the declarations for the [NEON intrinsics supported by GCC](https://gcc.gnu.org/onlinedocs/gcc-4.6.1/gcc/ARM-NEON-Intrinsics.html), including all the instructions and data types used in NEON.

NEON intrinsics can be used with datatypes named in the following pattern:  

```  
<type><size>x<number of lanes>_t
```

For example:

*   `int32x2_t` - vector with 2 lanes containing signed 32-bit integers
*   `float32x4_t` - quad vector with 4 lanes containing 32-bit floating-point numbers

### Naming and usage syntax for NEON instructions

Each NEON intrinsic name uses the following notational convention:  

```  
<opname>[q]_<type>
```

*   `q` specifies that the intrinsic operates on a quad (128-bit) vector
*   `<opname>` indicates the basic operation of the intrinsic; for example, `vadd` for addition and `vmul` for multiplication
*   `<type>` indicates the type of data the operation works on; for example, `s32` for signed 32-bit integer values, `f32` for 32-bit floating-point values

### Frequently used NEON intrinsics

#### Load operations:

```c
  float32x4_t vld1q_dup_f32(float32_t const * ptr)  // Load one SP FP value into all four words
  float32x4_t vld1q_f32(float32_t const * ptr)      // Load four SP FP values, the address must be 16-byte-aligned 
```

#### Set operations:

```c
  float32x4_t vmovq_n_f32(float32_t w);           // Set the four SP FP values to w 
```

#### Store operations:

```c
  void  vst1q_f32(float32_t * ptr, float32x4_t val);    // Stores the four SP FP values. The address must be 16-byte-aligned
```

##### Arithmetic operations:

```c
  float32x4_t vaddq_f32(float32x4_t a, float32x4_t b);   // Add the corresponding SP FP values in a and b element by element
  float32x4_t vmlaq_f32(float32x4_t a, float32x4_t b, float32x4_t c); // FP Multiply-Accumulate. Returns a + b*c
```

### Vector addition with NEON

*   Use the vector addition code provided in the SSE example along with the NEON code provided in the lecture slides to write a version using NEON intrinsics. To port your files to the ARM platform, remove the -msse2 flag in the Makefile and change the include of `emmintrin.h` to `arm_neon.h` in `vector_add.cpp` etc.
    
    _Note:_ as PAPI is not working on our ARM platform, you will need to comment out the `fprintf(...); exit(1);` lines in `simple_papi.cpp`.
    
*   Compile the code with option `-fopt-info-vec-all=dumpfile` on the ARM platform. The compiler now writes a file called dumpfile in the current directory, which reports all of the compiler's vectorization attempts, both successful and unsuccessful. In previous versions of gcc, it was possible to output this information to standard out during compilation using the `-ftree-vectorizer-verbose=1` option.
*   Run this code on the ARM platform and complete the following table
    
            Execution time and MFLOPS of vector add on the ARM system
        ----------------------------------------------------------------
        |                    |        | exec. time(ms) |    MFLOPS     |
        |--------------------|--------|----------------|---------------|
        |vector_add 2 8192   | Add    |                |               |
        |                    | Unroll |                |               |
        |                    | SSE    |                |               |
        |--------------------|--------|----------------|---------------|
        |vector_add 256 1024 | Add    |                |               |
        |                    | Unroll |                |               |
        |                    | SSE    |                |               |
        |--------------------|--------|----------------|---------------|
        |vector_add 2048 64  | Add    |                |               |
        |                    | Unroll |                |               |
        |                    | SSE    |                |               |
        ----------------------------------------------------------------
    
        
    
*   Compare the performance with Intel SSE2 code performance you measured earlier. Is it in line with your expectations?

### Vector dot product with NEON

*   Use the vector dot product code developed in the SSE session to write a version using NEON intrinsics.
*   Run this code on the ARM platform and complete the following table
    
           
                 Execution time and MFLOPS of vector dot product
          ----------------------------------------------------------------
          |                    |        | exec. time(ms) |    MFLOPS     |
          |--------------------|--------|----------------|---------------|
          |vector_dot 2 8192   | Dot    |                |               |
          |                    | Unroll |                |               |
          |                    | NEON   |                |               |
          |--------------------|--------|----------------|---------------|
          |vector_dot 256 1024 | Dot    |                |               |
          |                    | Unroll |                |               |
          |                    | NEON   |                |               |
          |--------------------|--------|----------------|---------------|
          |vector_dot 2048 64  | Dot    |                |               |
          |                    | Unroll |                |               |
          |                    | NEON   |                |               |
          ----------------------------------------------------------------
    
        
    
*   Compare the performance with Intel SSE2 code performance you measured earlier. Is it in line with your expectations?
