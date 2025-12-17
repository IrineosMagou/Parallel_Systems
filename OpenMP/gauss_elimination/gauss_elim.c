/* Purpose:
 *  Solves a system of linear equations Ax = b using Gaussian 
 *  elimination.The function performs forward elimination to 
 *  transform the matrix into upper triangular form, followed 
 *  by back substitution to compute the solution vector x. 
 *  It supports both serial execution and parallel.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "../../helpers/my_rand.h"
#include "../../helpers/timer.h"

#include "helpers/helpers.h"

/* ----------------- Gaussian Elimination Function ----------------- */
void gauss_elimination(double **A, double b[], double x[], int n, int num_of_threads, int serial){
    int i, j, k, col;
    double ratio;
    double start, finish;

    if (serial == 0) { // Parallel OpenMP execution
        GET_TIME(start);

// --- Forward Elimination ---
// Outer loop cannot be parallelized due to data dependency on 'i'
        for (i = 0; i < n - 1; i++) {
// The inner 'j' loop (row operations) can be parallelized
#pragma omp parallel for private(j, ratio, k) shared(A, b, n, i) num_threads(num_of_threads)
            for (j = i + 1; j < n; j++) {
                ratio = A[j][i] / A[i][i]; 
                for (k = i; k < n; k++) {
                    A[j][k] -= (ratio * A[i][k]); 
                }
                b[j] -= (ratio * b[i]); 
            }
        }

// --- Back Substitution ---
        for (int row = n - 1; row >= 0; row--) {
            double temp_sum = b[row];
 // Parallel computation of the sum
#pragma omp parallel for private(col) shared(A, x, row, n) reduction(- : temp_sum) num_threads(num_of_threads)
            for (col = row + 1; col < n; col++) {
// Reduction will calculate temp_sum = temp_sum - (A[row][col] * x[col])
                temp_sum -= A[row][col] * x[col];
            }
            x[row] = temp_sum / A[row][row];
        }
        GET_TIME(finish);
        printf("Parallel Gauss Elimination took %e\n", finish - start);

    } else { // Serial execution
        GET_TIME(start);
// --- Forward Elimination ---
        for (i = 0; i < n - 1; i++)
            for (j = i + 1; j < n; j++) {
                ratio = A[j][i] / A[i][i];
                for (k = i; k < n; k++)
                    A[j][k] -= (ratio * A[i][k]);
                b[j] -= (ratio * b[i]);
            }
// --- Back Substitution ---
        for (int row = n - 1; row >= 0; row--){
            x[row] = b[row];
            for (col = row + 1; col < n; col++)
                x[row] -= A[row][col] * x[col];
            x[row] /= A[row][row];
        }
        GET_TIME(finish);
        printf("Serial Gauss Elimination %e\n", finish - start);
    }
}


/* ----- Serial Execution for Result Validation and Speed-up Comparison -----*/
void serial_gauss(double **A, double **B, int n, double b0[], double x0[], double x[], double b[]){   
    int i, j, k, col;
    double ratio; 
// --- Forward Elimination on B and b0 (Validation/Reference system) ---
    for (i = 0; i < n - 1; i++){
        for (j = i + 1; j < n; j++){
// B is used here for the reference calculation
            ratio = B[j][i] / B[i][i];
            for (k = i; k < n; k++) {
                B[j][k] -= (ratio * B[i][k]);
            }
            b0[j] -= (ratio * b0[i]);
        }
    }

// --- Back Substitution for x0 ---
    for (int row = n - 1; row >= 0; row--){
        x0[row] = b0[row];
        for (col = row + 1; col < n; col++) {
            x0[row] -= B[row][col] * x0[col];
        }
        x0[row] /= B[row][row];
    }

// --- Validation against parallel results ---
    int check;
    if ((check = Validation(n, A, B)) == 0)
        printf("Something went wrong with the Matrix/RHS reduction (Validation)\n");
    
    if ((check = Results_validation(n, x, x0)) == 0)
        printf("Something went wrong with the Solution vector (Results_validation)\n");
    
    if ((check = Results_validation(n, b, b0)) == 0)
        printf("Something went wrong with the Modified RHS vector (Results_validation)\n");
}