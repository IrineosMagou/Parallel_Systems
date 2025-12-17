/* Purpose:
 *      Provides fucntions to help visualize and
 *      validate the results of the gauss elimination.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h> //for the array comparison(results)

#include "helpers.h"

/* -------------------- Usage / Help -------------------- */
void Usage(char *prog_name) {
    fprintf(stderr, "Usage: %s <equations> <threads>\n", prog_name);
    exit(0);
}

void Print_mtx(int n, double *A[]){
    int i, j;
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++)
            printf("%.2f ", A[i][j]);
        printf("\n");
    }
}

void Print_vct(int n, double x[]){
    int i;
    for (i = 0; i < n; i++)
        printf("%.2f ", x[i]);
}

int Validation(int n, double *A[], double *B[]){
    const double epsilon = 1e-8;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (fabs(A[i][j] - B[i][j]) > epsilon)
                return 0;
    return 1;
}

int Results_validation(int n, double x[], double x0[]){
    const double epsilon = 1e-8;
    for (int i = 0; i < n; i++)
        if (fabs(x[i] - x0[i]) > epsilon)
            return 0;
    return 1;
}