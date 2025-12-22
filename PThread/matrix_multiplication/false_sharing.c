/* Purpose:  
 *     Computes a parallel matrix-matrix product(m x n)(n x p). First Matrix
 *     is distributed by block rows. Second Matrix is distributed by 
 *     blocks(columns). A random number generator is used to
 *     generate matrices. 
 * Output:
 *     Elapsed time for the initialization
 *     Elapsed time for the computation
 *
 * Compile:  
 *     1. gcc -Wall -o executable false_sharing.c ../../helpers/matrix_mul_helpers -lpthread
 *     2. make build SRC=false_sharing.c
 * Usage:
 *     1. ./executable <m> <n> <p> <thread_number>
 *     2. make run m=<m> n=<n> p=<p> t=<thread_number>
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 
 *     and get aggregated execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <m> <n> <p>
 * Notes:  
 *     The main problem and showcase here is the false sharing
 *     problem that occurs when multiple threads update different 
 *     variables that reside in the same cache line, leading to 
 *     unnecessary cache invalidations and performance degradation.
 *     Despite splitting the array in row-blocks for each thread
 *     and no data overlaping, at the product matrix where all the
 *     threads write simultaneously the results, even if that is at
 *     different Matrix indices, false sharing arises.   
 *     Thread Number should divide the dimension of matrix
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../helpers/timer.h"
#include "helpers/matrix_mul_helpers.h"

/* ------------------ Global Variables ------------------ */
int thread_count;
int m, n, p;           // Matrix dimensions: A(m x n), B(n x p), C(m x p)
double *A, *B, *C;     // Matrices for multiplication
double start, finish;

/* ----------------- Function Prototypes ----------------- */
void *parallel_product_computation(void *rank);

/* -------------- Parallel Matrix Multiplication -------------- */
void *parallel_product_computation(void *rank){
    long my_rank = (long)rank;
    int local_m = m / thread_count;          // Number of rows assigned to this thread
    int my_first_row = my_rank * local_m;
    int my_last_row = my_first_row + local_m;

    double temp;

    GET_TIME(start);
    for (int i = my_first_row; i < my_last_row; i++){
        for (int k = 0; k < p; k++){
            C[i * p + k] = 0.0; //"clean" whatever junk happened to be in the memory
            for (int j = 0; j < n; j++){
                temp = A[i * n + j] * B[j * p + k];
                C[i * p + k] += temp;
            }
        }
    }
    GET_TIME(finish);

// Uncomment to debug per-thread timing
    // printf("Thread %ld elapsed time = %e seconds\n", my_rank, finish - start);

    return NULL;
}

/* ---------------------------- Main Function ---------------------------- */
int main(int argc, char *argv[]){
// --- 1. Argument Validation and Parsing ---
    if (argc != 5) usage(argv[0]);

    m = strtol(argv[1], NULL, 10);
    n = strtol(argv[2], NULL, 10);
    p = strtol(argv[3], NULL, 10);
    thread_count = strtol(argv[4], NULL, 10);
    
    if (n % thread_count != 0){
        fprintf(stderr, "The threads number should evenly divide n");
        exit(EXIT_FAILURE);
    }
// --- 2. Allocate Matrices ---
    GET_TIME(start);
    A = malloc(m * n * sizeof(double));
    B = malloc(n * p * sizeof(double));
    C = malloc(m * p * sizeof(double));
    GET_TIME(finish);
    printf("Elapsed initialization time = %e seconds\n", finish - start);

// --- 3.  Generate Random Matrices --- 
    gen_matrix(A, m, n);
    gen_matrix(B, n, p);

// --- 4. Create Threads and Join Threads --- 
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

    GET_TIME(start);
    for (long thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, parallel_product_computation, (void *)thread);

    for (long thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    GET_TIME(finish);

// Uncomment for comparison
    // D = malloc(m * p * sizeof(double)); // for serial code
    // serial_product_computation(A, B, D, m, n, p); // This is the serial multiplication , to check that results are the same , and to compare time
    // if(!results_validation(m*p, C, D)){
    //      fprintf(stderr, " The parallel matrix product is wrong");
    //      exit(0);
    // }
    printf("Parallel Execution Time: %e seconds\n", finish - start);

// --- 5. Free Memory --- 
    free(A);
    free(B);
    free(C);
    free(thread_handles);

    return EXIT_SUCCESS;
}
