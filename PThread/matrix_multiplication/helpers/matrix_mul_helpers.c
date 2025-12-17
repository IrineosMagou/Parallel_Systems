#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../../helpers/timer.h"


/* -------------------- Usage / Help -------------------- */
void Usage(char *prog_name) {
    fprintf(stderr, "Usage: %s <m> <n> <p> <thread_count>\n", prog_name);
    exit(0);
}

/* -------------------- Results Validation -------------------- 
 * Takes two matrices and compares the elements one-by-one. 
 */
 int Results_validation(int n, double x[], double x0[]){
    const double epsilon = 1e-9;

    for (int i = 0; i < n; i++)
        if (fabs(x[i] - x0[i]) > epsilon)
            return 0;
    return 1;
}

/* -------------------- Print Matrix -------------------- 
 * Example Usage: Print_matrix("We generated", A, m, n);
 */
void Print_matrix(char *title, double A[], int m, int n) {
    printf("%s\n", title);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            printf("%6.3f ", A[i * n + j]);
        printf("\n");
    }

}
/* ------------------ Generate Random Matrix ------------------ */
void Gen_matrix(double A[], int m, int n){

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            A[i * n + j] = random() / ((double)RAND_MAX);
} 
/* -------------- Serial Matrix Multiplication -------------- */
void serial_product_computation(double A[], double B[], double D[], int n, int m, int p){
    double start, finish;
    
    GET_TIME(start);
    for (int i = 0; i < m; i++)     // how many rows
        for (int k = 0; k < p; k++) // multiply with columns
        {
            D[i * p + k] = 0.0;

            for (int j = 0; j < n; j++) // for n elements(rows of A and columns of B)
            {
                D[i * p + k] += A[i * n + j] * B[j * p + k];
            }
        }
    GET_TIME(finish);
    printf("Serial Execution Time = %e seconds\n", finish - start);
}