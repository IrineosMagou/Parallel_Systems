# 🧵 OpenMP Overview
This directory solves problems using the **OpenMP** library.

**OpenMP** (Open Multi-Processing) is a high-level API for shared-memory parallel programming in C, C++, and Fortran. It enables parallel execution through **compiler directives (pragmas)**, runtime routines, and environment variables, allowing developers to parallelize code with *minimal changes* and without explicit thread management.
**OpenMP** uses compiler directives, commonly called pragmas, to specify parallel behavior directly in the source code. These directives are interpreted by the compiler and instruct it how to parallelize specific regions of a program.

**Key Directives and Constructs**

- ***#pragma omp parallel***: Creates a parallel region where a team of threads executes the enclosed code concurrently.
- ***#pragma omp for***: Distributes loop iterations across threads in a parallel region. This is commonly used to speed up computationally intensive loops with independent iterations.
- ***Reduction (reduction(...))***: Performs safe parallel accumulation of variables by creating private copies for each thread and combining them at the end of execution.
- ***Critical Sections (#pragma omp critical)***: Ensures that only one thread at a time executes a block of code. Used to protect shared data and prevent race conditions.


## Problems

#### 1. 🎯 **Compute pi with Monte-Carlo approach**
Suppose we throw darts at a square target with sides measuring 2 meters, with the center of the 
target being the origin (0,0) of the coordinate system. Suppose also that a circle is inscribed in this square target . The radius of the circle is 1 meter and its area is π square meters. If the points where the darts land are uniformly distributed (and the darts always hit the square target), then the number of arrows that hit the interior of the circle should approximately satisfy the equation:  
            ``` circle_arrows  / number_of_throws = π / 4 ```

We can use this equation to calculate an estimate for the value of π using a random number generator.
```
circle_arrows = 0; 
for (throw = 0; throw < number_of_throws; throw++) {
    x = random double between −1 and 1;
    y = random double between −1 and 1; 
    distance_square = x*x + y*y; 
    if (distance_square <= 1)  
        circle_arrows++; 
} 
estimate_π = 4*circle_arrows/((double) number_of_throws);
```

#### 2. 🧮 **Upper Triangular Matrix–Vector Multiplication**
Given the dimensions of a matrix, we randomly generate the matrix and a vector initializing them with *double* values. The matrix is divided between the threads by rows.
Two different OpenMP implementations are provided:

- ***Full Matrix Multiplication***
In the first source file, the multiplication is performed for every matrix element, including entries that are known to be zero. This approach follows the standard matrix–vector multiplication algorithm and serves as a simple, baseline implementation.
- ***Upper Triangular Optimization***
In the second source file, the computation is restricted to only the non-zero elements of the upper triangular matrix. Since all elements below the main diagonal are zero, they are skipped, reducing the number of operations and improving performance.

By parallelizing both versions with OpenMP, this project demonstrates how exploiting matrix structure (upper triangular form) can significantly reduce unnecessary computations while benefiting from multi-threaded execution.

#### 3. 📐 **Solving Linear Systems with Gaussian Elimination**
Gaussian elimination is a fundamental numerical algorithm used to solve systems of linear equations of the form:
```
Ax=b
```
where:

- A is a square matrix of coefficients,
- b is a known right-hand-side vector,
- x is the unknown solution vector.

The goal is to systematically transform the system into a simpler equivalent form that can be solved efficientl
Gaussian elimination consists of two main phases:

***Phase 1: Forward Elimination***

In the forward elimination phase, the original matrix is transformed into an upper triangular matrix, meaning that all elements below the main diagonal are reduced to zero.
Conceptually, this is done by:

1. Selecting a ***pivot*** element on the diagonal.
2. Using the pivot row to eliminate the entries below it in the same column.
3. Repeating this process column by column until the matrix is triangular.

At the end of this phase, the system has the form:
```
𝑎11*𝑥1 +𝑎12*𝑥2 + …=𝑏1
       𝑎22*𝑥2 + …=𝑏2
              ⋱
          𝑎𝑛𝑛*𝑥n =𝑏n
```
**Parallelization with OpenMP**
The elimination process has *data dependencies*:
Each pivot row must be computed before it can be used to eliminate rows below it. As a result:

- The outer loop over pivot rows is executed serially
- The row update operations below the pivot are independent and can be parallelized

OpenMP is used to parallelize these independent row updates, allowing multiple threads to eliminate different rows simultaneously while preserving correctness.

***Phase 2: Back Substitution***
Once the matrix is in upper triangular form, the solution vector is computed using back substitution.
This phase works from the last equation upward:

1. Solve for the last unknown directly.
2. Substitute this value into the previous equation.
3. Continue until all unknowns are computed.

For each equation, the solution involves a summation of known values.

**Parallelization with OpenMP**
Although the equations themselves must be solved sequentially, the inner summation for each equation is independent. OpenMP is used with a reduction clause to parallelize this summation, allowing multiple threads to compute partial sums efficiently.
This reduces computation time while respecting the inherent dependencies between equations.


The OpenMP implementation parallelizes only the independent computations, ensuring correctness while improving performance on multi-core systems. A serial version is also provided for result validation and performance comparison, demonstrating the speedup achieved through parallel execution.