/* Purpose:
 *     Solves linear system equation by applying the gauss-elimination
 *     method.
 *
 * Compile:
 *    1. gcc -g -Wall -o executable gauss_elim.c gauss_main.c helpers/helpers.c ../../helpers/my_rand.c -fopenmp
 *    2. make
 * Run:
 *    1. ./executable <num_of_equations> <thread_num> 
 *    2. make run n=<num_of_equations> t=<thread_num>
 * Output:
 *    Elapsed time for the parallel or serial computation
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 and get aggregated 
 *     execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <num_of_equations>
 * Notes:
 *    thread_num could be given as 1 for the code to run serial
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "../../helpers/my_rand.h"
#include "../../helpers/timer.h"

#include "helpers/helpers.h"

/* ---------------------- Main Function ----------------------- */
int main(int argc, char *argv[]){
// --- 1. Argument Validation and Parsing ---
    if (argc != 3) Usage(argv[0]);
    double ratio, start, finish;
    unsigned seed = 1, rnd;
    int i, j, k, col, serial=0; 
    int n = atoi(argv[1]); //size of the linear system
    int num_of_threads = atoi(argv[2]);
    if (num_of_threads == 1) serial = 1; //flag for serial execution
//x: solution vector, b: Right Hand Side(RHS) 
    double x_parallel[n], b_parallel[n], b_serial[n], x_serial[n];

// --- 2. Linear System Initialization --- 
// B is for validation
    double **A = (double **)malloc(n * sizeof(double *));
    double **B = (double **)malloc(n * sizeof(double *));

    for (int i = 0; i < n; i++){
        A[i] = (double *)malloc(n * sizeof(double));
        B[i] = (double *)malloc(n * sizeof(double));
    }

    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            A[i][j] = my_drand(&seed);
            B[i][j] = A[i][j];
        }
        b_parallel[i] = my_drand(&seed);
        b_serial[i] = b_parallel[i];
    }
// --- 3. Invoke OpenMP function ---
    gauss_elimination(A, b_parallel, x_parallel, n, num_of_threads, serial);

// Uncomment for time comparison and result validation
    // serial_gauss(A, B, n, b0, x_serial, x, b);

// --- 4. Free Memory --- 
    for (int i = 0; i < n; i++){
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
    return EXIT_SUCCESS;
}

   