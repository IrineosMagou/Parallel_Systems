# Parallel Systems/Programming with C

This repository contains  a series of projects that explore **parallel programming** by solving computational problems using different parallelization approaches.

**Parallel Programming** is a software development technique where *multiple tasks execute simultaneously* rather than sequentially. This enables programs to run faster by leveraging **multiple CPU-cores, hardware threads**, or even **clusters of machines** working together.


Each subdirectory demonstrates a different method of parallelization in C, including:
- **Pthreads** -- Low-level thread creation and management.
- **OpenMP**   -- Compiler-assisted parallelism using pragmatic directives.
- **MPI**      -- Message passing for distributed systems and multi-node parallelism.

These projects highlight the **strengths, use cases and trade-offs** of each approach. They also illustrate common challenges in parallel computing, such as:
- Memory sharing and race conditions
- Synchronization mechanisms
- Load balancing and work distribution
- Communication overhead(especially in distributed systems)

The goal of this repository is to provide clear, practical examples of how different parallel programming models work in C and how they can be applied to real computational problems.

In each subdirectory you can find a **README.md** file for the specific parallelization method, informations about the problems and the solutions. **Makefile** for easier compilation and execution of the programs. You can also find a **bash script** for results aggregation and speed comparison. These scripts run the source code multiple times for 2-8 threads.



