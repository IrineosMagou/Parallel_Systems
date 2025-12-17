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

##### 1. **Compute pi with Monte-Carlo approach**
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

##### 2. **Upper Triangular Matrix–Vector Multiplication**
Given the dimensions of a matrix, we randomly generate the matrix and a vector initializing them with *double* values. The matrix is divided between the threads by rows.
Two different OpenMP implementations are provided:

- ***Full Matrix Multiplication***
In the first source file, the multiplication is performed for every matrix element, including entries that are known to be zero. This approach follows the standard matrix–vector multiplication algorithm and serves as a simple, baseline implementation.
- ***Upper Triangular Optimization***
In the second source file, the computation is restricted to only the non-zero elements of the upper triangular matrix. Since all elements below the main diagonal are zero, they are skipped, reducing the number of operations and improving performance.

By parallelizing both versions with OpenMP, this project demonstrates how exploiting matrix structure (upper triangular form) can significantly reduce unnecessary computations while benefiting from multi-threaded execution.

##### 3. **Solving Linear Systems with Gaussian Elimination**
Gaussian elimination consists of two main phases:

- ***Forward Elimination***
The coefficient matrix is transformed into an upper triangular form by eliminating elements below the main diagonal.
Due to data dependencies between pivot rows, the outer loop is executed serially, while the row update operations are parallelized using OpenMP.
- ***Back Substitution***
Once the matrix is upper triangular, the solution vector is computed starting from the last equation and moving upward.
The inner summation for each row is parallelized using an OpenMP reduction, allowing multiple threads to compute partial sums efficiently.

The OpenMP implementation parallelizes only the independent computations, ensuring correctness while improving performance on multi-core systems. A serial version is also provided for result validation and performance comparison, demonstrating the speedup achieved through parallel execution.