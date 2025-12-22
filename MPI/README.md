# 🌐 MPI Overview

This directory solves problems using the **MPI library**.

**MPI (Message Passing Interface)** is a standardized and portable API for distributed-memory parallel programming in C, C++, and Fortran. It enables parallel execution by allowing multiple independent processes to communicate explicitly through message passing. **MPI** requires developers to manage data distribution and communication explicitly, making it well-suited for large-scale systems such as clusters and supercomputers.

**MPI** programs follow the *Single Program, Multiple Data (SPMD)* model, where the same program runs on multiple processes. Each process has its own private memory space, and coordination is achieved through communication routines provided by the **MPI library**.

**Key Concepts and Routines**

- ***MPI_Init / MPI_Finalize***: Initialize and shut down the MPI execution environment. All MPI programs must begin with MPI_Init and end with MPI_Finalize.
- ***MPI_Comm_size***: Determines the total number of processes participating in a communicator (typically MPI_COMM_WORLD).
- ***MPI_Comm_rank***: Identifies the unique rank (ID) of each process within a communicator. Ranks are commonly used to assign different portions of work.
- ***Point-to-Point Communication (MPI_Send / MPI_Recv)***: Enables explicit data exchange between processes. Used to distribute data, gather results, or synchronize execution.
- ***Collective Communication (e.g., MPI_Bcast, MPI_Scatter, MPI_Gather, MPI_Reduce)***: Involves all processes in a communicator to perform common communication patterns such as broadcasting data, distributing workloads, collecting results, or performing global reductions.

## Problems

##### 1. 🎯 **Compute pi with Monte-Carlo approach**
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

##### 2. 🧮 **Parallel Matrix–Vector Multiplication**
The Matrix is partitioned into equally sized square blocks and each process is responsible for a square.The program assumes that the total number of MPI processes is a perfect square.

***Each MPI process is responsible for***:
- One of the submatrix(squares) of the global Matrix
- The corresponding segment of the input vector v
- Computing a partial result of the output vector p

***MPI Communication and Data Distribution:***
Initialization and Input Handling

- ***MPI_Init, MPI_Comm_size, and MPI_Comm_rank*** initialize MPI and determine process identifiers.
- ***Rank 0*** reads the matrix dimension, validates constraints, and initializes the global matrix and vector.
- ***MPI_Bcast*** is used to broadcast the local block dimension (local_dim) to all processes.

***Matrix Distribution***

- The global matrix is distributed using ***MPI_Scatterv***.
- A derived MPI datatype ***(MPI_Type_create_subarray)*** represents a 2D submatrix block.
- ***MPI_Type_create_resized*** ensures correct memory spacing between blocks.
- ***Rank 0*** computes displacement and send-count arrays to scatter one submatrix block to each process.

***Vector Distribution***

- The input vector is distributed using ***MPI_Scatterv***
- Each process receives the portion of the vector corresponding to the columns of its local matrix block.

***Local Computation***

Each process performs a local matrix–vector multiplication

***Reduction and Result Assembly***

- Processes are grouped into row communicators using ***MPI_Comm_split***.
- Within each row of the process grid, partial results are combined using ***MPI_Reduce_scatter_block***, summing contributions from blocks in the same matrix row.
- The reduced segments are gathered on rank 0 using ***MPI_Gather*** to form the complete result vector p.

***Finalization***

- All dynamically allocated memory, derived datatypes, and communicators are freed.
- The program terminates with ***MPI_Finalize***.
