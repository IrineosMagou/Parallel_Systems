#ifndef _HELPERS_H
#define _HELPERS_H

int malloc2double(double ***array, int n);
int free2double(double ***array);
void gen_mtx_vct(double **global , double *vector_v , int gridsize );
void print_matrix(double **global , int n);
void mtx_vct_mul(double A[], double x[], double y[], int n);
int results_validation(int n, double x[], double x0[]);
void usage(char *prog_name);

#endif