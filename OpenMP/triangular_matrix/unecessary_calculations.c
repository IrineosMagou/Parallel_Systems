/* Purpose:
 *     Computes a parallel matrix-vector product.  Matrix
 *     is distributed by block rows.  Vectors are distributed by
 *     blocks.  This version uses a random number generator to
 *     generate A and x. 
 * Compile:
 *    1. gcc -g -Wall -o executable only_necessary.c helpers/helpers.c -fopenmp 
 *    2. make build SRC=filename.c
 * Run:
 *    1. ./executable <thread_count> <m> <n>
 *    2.  make run t=<t> m=<m> n=<n>
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 
 *     and get aggregated execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <m> <n>
 * Input:
 *    None unless compiled with DEBUG flag.
 *    With DEBUG flag, A, x
 *
 * Output:
 *    y: the product vector
 *    Elapsed time for the computation
 *
 * Notes:
 *     Number of threads (thread_count) should evenly divide
 *     both m and n. The program doesn't check for this.
 *     This program calculates the product for all the elements 
 *     of the matrix, leading to unecessary calculations
 *     because the elements under the main diagonal are all zero.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "../../helpers/timer.h"
#include "helpers/helpers.h"

/* -------------------- Function Prototypes -------------------- */
void omp_mat_vect(const double A[], const double x[], double y[],
                  int m, int n, int thread_count);

/* ---------------------- Main Function ----------------------- */
int main(int argc, char *argv[]){
// --- 1. Argument Validation and Parsing ---
    int thread_count, m, n;
    get_args(argc, argv, &thread_count, &m, &n);

// --- 2. Allocate Matrices ---
    double *A = NULL;
    double *x = NULL;
    double *y = NULL;

    A = malloc((size_t)m * n * sizeof(double));
    x = malloc((size_t)n * sizeof(double));
    y = malloc((size_t)m * sizeof(double));

    if (!A || !x || !y) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(A); free(x); free(y);
        return EXIT_FAILURE;
    }

#ifdef DEBUG
    read_matrix("Enter the matrix", A, m, n);
    print_matrix("We read", A, m, n);
    read_vector("Enter the vector", x, n);
    print_vector("We read", x, n);
#else
    gen_matrix(A, m, n);
    gen_vector(x, n);
#endif
// --- 3. Invoke OpenMP function ---
    omp_mat_vect(A, x, y, m, n, thread_count);

#ifdef DEBUG
pPrint_vector("The product is", y, m);
#endif
// --- 5. Free Memory  --- 
    free(A);
    free(x);
    free(y);

    return EXIT_SUCCESS;
}

/* ------------ Parallel Matrix x Vector Multiplication ------------ */
void omp_mat_vect(const double A[], const double x[], double y[],
                  int m, int n, int thread_count){
    double start, finish;

    GET_TIME(start);
    int i,j;
#pragma omp parallel for num_threads(thread_count) \
    default(none) private(i,j) shared(A,x,y,m,n) schedule(static)
    for (i = 0; i < m; i++) {
        double sum = 0.0;              // thread-local
        for (j = 0; j < n; j++) // FIXED: correct full row
            sum += A[i * n + j] * x[j];

        y[i] = sum;
    }

    GET_TIME(finish);

    printf("Elapsed Time: %.6e seconds\n", finish - start);
}
