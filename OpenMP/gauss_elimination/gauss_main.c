/* Purpose:
 *     Solves linear system equation by applying the gauss-elimination
 *     method.
 *
 * Compile:
 *    gcc -g -Wall -o executable gauss_elim.c gauss_main.c helpers/helpers.c ../../helpers/my_rand.c -fopenmp
 * Run:
 *    ./executable <equation> <thread_num> <serial_flag>
 * Output:
 *    Elapsed time for the parallel computation
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
    if (argc != 4) Usage(argv[0]);
    double ratio, start, finish;
    unsigned seed = 1, rnd;
    int i, j, k, col; 
    int n = atoi(argv[1]); //size of the linear system
    int num_of_threads = atoi(argv[2]);
    int serial = atoi(argv[3]); //flag for serial execution
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

   