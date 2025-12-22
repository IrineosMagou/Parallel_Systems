#ifndef _MATRIX_HELPERS_H
#define _MATRIX_HELPERS_H

void usage(char *prog_name);
int results_validation(int n, double x[], double x0[]);
void print_matrix(char *title, double A[], int m, int n);
void gen_matrix(double A[], int m, int n);
void serial_product_computation();


#endif

