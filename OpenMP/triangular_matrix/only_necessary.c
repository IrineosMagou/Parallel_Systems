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
 * Output:
 *    y: the product vector
 *    Elapsed time for the computation
 * Notes:
 *     Number of threads (thread_count) should evenly divide
 *     both m and n.  The program doesn't check for this.
 *     Inside the product loop, j >= i is used in order to
 *     skip the elements below the main diagonal that are zero.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "../../helpers/timer.h"
#include "helpers/helpers.h"

/* -------------------- Function Prototypes -------------------- */
void Omp_mat_vect(const double A[], const double x[], double y[],
                  int m, int n, int thread_count);

/* ---------------------- Main Function ----------------------- */
int main(int argc, char *argv[]){
// --- 1. Argument Validation and Parsing ---

    int thread_count, m, n;
    Get_args(argc, argv, &thread_count, &m, &n);

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
    Read_matrix("Enter the matrix", A, m, n);
    Print_matrix("We read", A, m, n);
    Read_vector("Enter the vector", x, n);
    Print_vector("We read", x, n);
#else
    Gen_matrix(A, m, n);
    Gen_vector(x, n);
#endif
// --- 3. Invoke OpenMP function ---
    Omp_mat_vect(A, x, y, m, n, thread_count);

#ifdef DEBUG
    Print_vector("The product is", y, m);
#endif
// --- 4. Free Memory --- 
    free(A);
    free(x);
    free(y);

    return EXIT_SUCCESS;
}

/* ------------ Parallel Matrix x Vector Multiplication ------------ */
void Omp_mat_vect(const double A[], const double x[], double y[],
                  int m, int n, int thread_count){
    double start, finish;

    GET_TIME(start);
    int i, j;
#pragma omp parallel for num_threads(thread_count)                 \
    default(none) private(i, j) shared(A, x, y, m, n) schedule(static)
    for (i = 0; i < m; i++) {
        double sum = 0.0;
// Upper triangular: only use j >= i
        for (j = i; j < n; j++) {
            sum += A[i * n + j] * x[j];
        }

        y[i] = sum;
    }

    GET_TIME(finish);

    printf("Elapsed Time: %e seconds\n", finish - start);
}
