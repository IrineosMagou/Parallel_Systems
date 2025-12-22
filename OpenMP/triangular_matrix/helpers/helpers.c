/* Purpose:
 *     Functions that help with matrix-vactor operations
 */

#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

/*------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get command line args
 * In args:   argc, argv
 * Out args:  thread_count_p, m_p, n_p
 */
void get_args(int argc, char *argv[], int *thread_count_p,
              int *m_p, int *n_p){

    if (argc != 4) usage(argv[0]);
    *thread_count_p = strtol(argv[1], NULL, 10);
    *m_p = strtol(argv[2], NULL, 10);
    *n_p = strtol(argv[3], NULL, 10);
    if (*thread_count_p <= 0 || *m_p <= 0 || *n_p <= 0)
        usage(argv[0]);
} 

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void usage(char *prog_name){
    fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", prog_name);
    exit(0);
}

/*------------------------------------------------------------------
 * Function:    Read_matrix
 * Purpose:     Read in the matrix
 * In args:     prompt, m, n
 * Out arg:     A
 */
void read_matrix(char *prompt, double A[], int m, int n){
    int i, j;

    printf("%s\n", prompt);
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            scanf("%lf", &A[i * n + j]);
} 

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void gen_matrix(double A[], int m, int n){
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++){
            if (i > j) // condition for the matrix to be
                A[i * n + j] = 0;
            else
                A[i * n + j] = random() / ((double)RAND_MAX);
        }
}

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void gen_vector(double x[], int n){
    int i;
    for (i = 0; i < n; i++)
        x[i] = random() / ((double)RAND_MAX);
}

/*------------------------------------------------------------------
 * Function:        Read_vector
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void read_vector(char *prompt, double x[], int n){
    int i;

    printf("%s\n", prompt);
    for (i = 0; i < n; i++)
        scanf("%lf", &x[i]);
}


/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void print_matrix(char *title, double A[], int m, int n){
    int i, j;

    printf("%s\n", title);
    for (i = 0; i < m; i++){
        for (j = 0; j < n; j++)
            printf("%4.1f ", A[i * n + j]);
        printf("\n");
    }
}

/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void print_vector(char *title, double y[], double m){
    int i;

    printf("%s\n", title);
    for (i = 0; i < m; i++)
        printf("%4.1f ", y[i]);
    printf("\n");
} 
