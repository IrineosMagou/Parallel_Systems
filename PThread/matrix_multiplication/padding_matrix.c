/* Purpose :
 *     Computes a parallel matrix-matrix product(m x n)(n x p). First Matrix
 *     is distributed by block rows. Second Matrix is distributed by 
 *     blocks(columns). A random number generator is used to
 *     generate matrices
 * Output:
 *     Elapsed time for the initialization
 *     Elapsed time for the computation(Serial or Parallel)
 * Compile:  
 *     1. gcc -Wall -o executable padding_matrix.c ../../helpers/matrix_mul_helpers -lpthread
 *     2. make build SRC=padding_matrix.c
 * Usage:
 *     1. ./executable <thread_count> <m> <n> <p> <thread_number>
 *     2. make run m=<m> n=<n> p=<p> t=<thread_number>
 * -------------------------------------------------------------------------------------------
 *  Notes:
 *     This program solves the false sharing problem by implementing
 *     the padding technique. By adding unused “padding” bytes around 
 *     per-thread data so that each thread’s data occupies its own 
 *     cache line, we ensure threads never touch the same line. This 
 *     keeps their updates independent and avoids false sharing.
 */    
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../helpers/timer.h"
#include "../../helpers/matrix_mul_helpers.h"
#include <math.h>

/* --- Define Padding Constants --- */
// Cache line size in bytes (e.g., 64 bytes)
#define CACHE_LINE_SIZE 64 
// Size of a double (8 bytes)
#define DOUBLE_SIZE sizeof(double)

// Calculate how many extra doubles are needed to pad to the next cache line boundary
// The row size must be a multiple of CACHE_LINE_SIZE / DOUBLE_SIZE (which is 8 doubles)
#define PADDING_DOUBLES (CACHE_LINE_SIZE / DOUBLE_SIZE)

/* ------------------ Global Variables ------------------ */
int thread_count;
int m, n, p;
// PADDED_P will store the new, padded column dimension for matrix C
int PADDED_P; 
double *A, *B, *C;
double start, finish;

/* ----------------- Function Prototypes ----------------- */
void *parallel_product_computation(void *rank);

/* -------------- Parallel Matrix Multiplication with Data Padding -------------- */
void *parallel_product_computation(void *rank) {
    long my_rank = (long)rank;
    int local_m = m / thread_count;
    int my_first_row = my_rank * local_m;
    int my_last_row = my_first_row + local_m;

    double temp;

    GET_TIME(start);
    for (int i = my_first_row; i < my_last_row; i++) {
// Use PADDED_P for indexing matrix C
        for (int k = 0; k < p; k++) {
// C[i * PADDED_P + k] is now guaranteed to be cache-aligned
            C[i * PADDED_P + k] = 0.0;
            for (int j = 0; j < n; j++) {
                temp = A[i * n + j] * B[j * p + k];
                C[i * PADDED_P + k] += temp;
            }
        }
    }
    GET_TIME(finish);
    // printf("Thread %ld > Elapsed time = %e seconds\n", my_rank, finish - start);

    return NULL;
}

/* ---------------------------- Main Function ---------------------------- */
int main(int argc, char *argv[]) {
// --- 1. Argument Validation and Parsing ---
    if (argc != 5) Usage(argv[0]);

    m = strtol(argv[1], NULL, 10);
    n = strtol(argv[2], NULL, 10);
    p = strtol(argv[3], NULL, 10);
    thread_count = strtol(argv[4], NULL, 10);
    
    if (m % thread_count != 0){
        fprintf(stderr, "Error: The threads number should evenly divide the number of rows (m).\n");
        exit(EXIT_FAILURE);
    }
    
// --- DETERMINE PADDED_P ---
// Calculate the padded column dimension for C (PADDED_P)
// Find the next multiple of PADDING_DOUBLES that is >= p
    PADDED_P = (p + PADDING_DOUBLES - 1) / PADDING_DOUBLES * PADDING_DOUBLES;

// --- 2. Allocate Matrices with Padding for C ---
    GET_TIME(start);
    A = malloc(m * n * sizeof(double));
    B = malloc(n * p * sizeof(double));
    // Allocate C using the padded column count: m x PADDED_P
    C = malloc(m * PADDED_P * sizeof(double)); 
    GET_TIME(finish);
    printf("Elapsed initialization time = %e seconds\n", finish - start);
    // Display padding information
    printf("Original P (columns): %d, Padded P: %d, Padding Doubles per row: %d\n", 
           p, PADDED_P, PADDED_P - p);

// --- 3.  Generate Random Matrices --- 
    Gen_matrix(A, m, n);
    Gen_matrix(B, n, p);
    // Note: C is allocated larger, but Gen_matrix doesn't need C.

// --- 4. Create Threads and Join Threads --- 
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

    GET_TIME(start);
    for (long thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, parallel_product_computation, (void *)thread);

    for (long thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    GET_TIME(finish);

printf("Parallel Execution Time: %e seconds\n", finish - start);

// --- 5. Free Memory --- 
    free(A);
    free(B);
    free(C);
    free(thread_handles);

    return EXIT_SUCCESS;
}