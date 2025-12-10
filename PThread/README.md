# 🧵 POSIX Threads (pthread) Overview

This directory solves three problems with the use of the **pthread** library.

The pthread library provides a standardized API for creating and managing threads in C/C++. It allows programs to perform multiple tasks concurrently within the same process, improving performance on multi-core systems.

**Key Functions**

- pthread_create()
Creates a new thread within the calling process. You specify the thread handle, attributes (or NULL), the function the thread will run, and its argument.

- pthread_join()
Waits for a specific thread to finish execution. This is commonly used to ensure that all threads complete before the program exits or to retrieve values returned by threads.

- Mutexes (pthread_mutex_*)
Mutexes (mutual exclusions) provide a locking mechanism to protect shared data from concurrent access.
Common functions:
    - pthread_mutex_init() – Initialize a mutex
    - pthread_mutex_lock() – Acquire the lock
    - pthread_mutex_unlock() – Release the lock
    - pthread_mutex_destroy() – Clean up the mutex

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

#####  2. **Matrix Multiplication**

Given the dimensions of two matrices, we randomly generate them initializing them with *double* values. First matrix(A) is divided between the threads by rows and the second matrix(B) by columns:

```
                        Rows
A = | A0 | <- thread 0   0               B =  | B0 | B1 | B2 | B3 | B5 |  ...
    | A1 | <- thread 1   1                       ^    ^    ^    ^         ...
    | A2 | <- thread 2   2               thread  0    1    2    3         ...
    | A3 | <- thread 3   3               Cols    0    1    2    3         ...
    | A4 | <- thread 0   4
                .
                .
                .
```

In this problem, the false sharing problem that arises when multiple threads update different variables that reside in the same cache line, leading to unnecessary cache invalidations and performance degradations.
We use two approaches to solve it. 
- By providing each thread with its local array to write the results.
- Using the padding technique:
 Adding unused "padding" bytes around per-thread data, each thread's data occupies it's own cache line, so no more than one thread touches the same line.

##### 3. **Readers and Writers** 
It's a classic synchronization problem used to illustrate and solve common concurrency issues, specifically dealing with shared resources where multiple processes or threads need to access the same data.

The Core Problem
The goal of the Readers-Writers problem is to allow multiple threads to access shared data simultaneously while maintaining data integrity, based on the type of operation:

- Readers: Threads that only read the data. Multiple readers can access the data concurrently without causing conflicts.
- Writers: Threads that modify (read and write) the data. Only one writer can access the data at any time, and when a writer is active, no other readers or writers are allowed access.

The challenge is designing a synchronization mechanism (using mutexes, semaphores, or condition variables) that enforces these rules while avoiding common pitfalls like deadlocks or starvation. 

Two approaches:
1. Giving priority to waiting Readers (Starving Writers)
    - A reader only blocks if a writer is currently in the critical section.
    - A reader is not blocked by waiting writers. This is the crucial point: if a writer is waiting, but no other writer is currently active , a new reader can immediately proceed without waiting or checking the writer queue. This allows readers to flow in and potentially starve writers.

2.  Giving priority to waiting Writers
    - A Reader is blocked if a Writer is either active OR waiting.
    - A Writer only blocks if a Reader is active OR another Writer is active.



