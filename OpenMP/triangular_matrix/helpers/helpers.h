/* The header file with the declarations of the variables, 
 * data structures and function names that all the source files use.
 */

#ifndef _HELPERS_H
#define _HELPERS_H

/* ----------------- Function Prototypes ----------------- */
void get_args(int argc, char *argv[], int *thread_count_p,
              int *m_p, int *n_p);
void usage(char *prog_name);
void gen_matrix(double A[], int m, int n);
void read_matrix(char *prompt, double A[], int m, int n);
void gen_vector(double x[], int n);
void read_vector(char *prompt, double x[], int n);
void print_matrix(char *title, double A[], int m, int n);
void print_vector(char *title, double y[], double m);


#endif
