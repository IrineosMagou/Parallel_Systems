#ifndef _HELPERS_H
#define _HELPERS_H

void Print_mtx(int n, double *A[]);
void Print_vct(int n, double x[]);
void Usage(char *prog_name);

int Validation(int n, double *A[], double *B[]);
int Results_validation(int n, double x[], double x0[]);
void gauss_elimination(double **A, double b[], double x[], 
                       int n, int num_of_threads, int serial);
void serial_gauss(double **A, double **B, int n, double b0[], 
                    double x0[], double x[], double b[]);

#endif
