/* Purpose :
 *     Computes a parallel matrix-matrix product(m x n)(n x p). First Matrix
 *     is distributed by block rows. Second Matrix is distributed by 
 *     blocks(columns). A random number generator is used to
 *     generate matrices
 * Output:
 *     Elapsed time for the initialization
 *     Elapsed time for the computation
 * Compile:  
 *     1. gcc -g -Wall -o executable local_matrix.c ../../helpers/matrix_mul_helpers -lpthread
 *     2. make build SRC=local_matrix.c
 * Usage:
 *     1. ./executable <thread_count> <m> <n> <p> <thread_number>
 *     2. make run m=<m> n=<n> p=<p> t=<thread_number>
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 
 *     and get aggregated execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <m> <n> <p> *  Notes:
 *     This program provides threads with their own local array (`my_A`) to 
 *     save the results of their computation block. This strategy isolates 
 *     the intensive calculation phase from the global result matrix (C), 
 *     effectively mitigating the problem of False Sharing. A mutex is used 
 *     only once per thread for the final, fast transfer via `memcpy`.
 *     Thread Number should divide the dimension of matrix
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../../helpers/timer.h"
#include "helpers/matrix_mul_helpers.h"

/* ------------------ Global Variables ------------------ */
int thread_count;
int m, n, p;
double *A, *B, *C, *D;
double start, finish, elapsed;
double *x;
pthread_mutex_t mutex_p;

/* ----------------- Function Prototypes ----------------- */
void *parallel_product_computation(void *rank);


/* -------------- Parallel Matrix Multiplication -------------- */
void *parallel_product_computation(void *rank){
    long my_rank = (long)rank;
    long int local_m = m / thread_count; // how many lines i need to take care
    int my_first_row = my_rank * local_m;
    int my_last_row = my_first_row + local_m;
    double *my_A = malloc(local_m * n * sizeof(double));

    // GET_TIME(start)
    for (int i = my_first_row; i < my_last_row; i++)
        for (int k = 0; k < p; k++){ // Column index of C and B
            double sum = 0.0; // Initialize sum for C[i][k]
            
            for (int j = 0; j < n; j++) // Inner product loop over n
                sum += A[i * n + j] * B[j * p + k];
            
// The index in my_A is relative to the start of the thread's block
            long local_row = i - my_first_row;
            my_A[local_row * p + k] = sum;
        }
    pthread_mutex_lock(&mutex_p);
    memcpy(&C[my_first_row * p], my_A, sizeof(double) * local_m * p);
    pthread_mutex_unlock(&mutex_p);
    // GET_TIME(finish)
    // printf("Thread %ld > Elapsed time = %e seconds\n", my_rank, finish - start);
    free(my_A);

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

    int mutexInit = pthread_mutex_init(&mutex_p, NULL);
    if (mutexInit != 0){
        printf("Something went wrong with mutex cration");
        return 0;
    }

// --- 2. Allocate Matrices ---
    GET_TIME(start);
    A = malloc(m * n * sizeof(double));
    B = malloc(n * p * sizeof(double));
    C = malloc(m * p * sizeof(double));
    GET_TIME(finish);
    printf("Elapsed initializon time = %e seconds\n", finish - start);

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
        
// --- 5. Free Memory & Destroy Mutex --- 
    pthread_mutex_destroy(&mutex_p);
    free(A);
    free(B);
    free(C);
    
    return EXIT_SUCCESS;
}