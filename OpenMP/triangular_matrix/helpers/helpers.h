/* The header file with the declarations of the variables, 
 * data structures and function names that all the source files use.
 */

#ifndef _HELPERS_H
#define _HELPERS_H

/* ----------------- Function Prototypes ----------------- */
void Get_args(int argc, char *argv[], int *thread_count_p,
              int *m_p, int *n_p);
void Usage(char *prog_name);
void Gen_matrix(double A[], int m, int n);
void Read_matrix(char *prompt, double A[], int m, int n);
void Gen_vector(double x[], int n);
void Read_vector(char *prompt, double x[], int n);
void Print_matrix(char *title, double A[], int m, int n);
void Print_vector(char *title, double y[], double m);


#endif
