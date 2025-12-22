#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"

/* -------------------- Usage / Help -------------------- */
void usage(char *prog_name) {
    fprintf(stderr, "Usage: %s <m> <n> <p> <thread_count>\n", prog_name);
    exit(0);
}

/* ------------- Generate Random Matrix-Vector ------------- */
void gen_mtx_vct(double **global , double *vector_v , int gridsize ){
    for (int i=0; i<gridsize; i++) {
        for (int j=0; j<gridsize; j++)
            global[i][j] = ((double)random()) / ((double)RAND_MAX) ;
        vector_v[i] = ((double)random()) / ((double)RAND_MAX);
    }
}

/* ---------------------- Print Matrix ---------------------- */
void print_matrix(double **global , int n){
    printf("Global array is:\n");
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++)
        printf( "%f " , global[i][j]);    
    printf("\n");
    }
}

/* -------------- Matrix-Vector Multiplication -------------- */
void mtx_vct_mul(double A[], double x[], double y[], int n){
    double temp;
    for (int i = 0; i < n; i++){
        y[i] = 0;
        for (int j = 0; j < n; j++){
            temp = A[i * n + j] * x[j];
            y[i] += temp;
        }
    }
}

/* -------------- Matrix Memory Allocation -------------- */
int malloc2double(double ***array, int n) {
    
    /* allocate the n*m contiguous items */
    double *p = (double *)malloc(n*n*sizeof(double));
    if (!p) return -1;

    /* allocate the row pointers into the memory */
    (*array) = (double **)malloc(n*sizeof(double*));
    if (!(*array)) {
       free(p);
       return -1;
    }

    /* set up the pointers into the contiguous memory */
    for (int i=0; i<n; i++)
       (*array)[i] = &(p[i*n]);
    
    return 0;
}

/* -------------- Matrix Memory De-Allocation -------------- */
int free2double(double ***array) {
    /* free the memory - the first element of the array is at the start */
    free(&((*array)[0][0]));

    /* free the pointers into the memory */
    free(*array);

    return 0;
}

/* -------------------- Results Validation -------------------- 
 * Takes two matrices and compares the elements one-by-one. 
 */
int results_validation(int n, double x[], double x0[]){
    const double epsilon = 1e-9;

    for (int i = 0; i < n; i++)
        if (fabs(x[i] - x0[i]) > epsilon)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}