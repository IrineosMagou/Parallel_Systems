/* Purpose:  
 *     Computes pi using the Monte Carlo approach. The total throws are
 *     distributed across threads equally and each thread calculates 
 *     it's own circle hits and then these hits are collected back
 *     to the main thread, summed up and calculate π.
 * Output:
 *     Serial and Parallel execution times
 *     Estimation of pi
 * Compile:  
 *     1. Terminal Command:
 *              gcc -Wall -o executable monte_carlo.c ../../helpers/my_rand.c -fopenmp
 *     2. Makefile:
 *              make
 * Usage:
 *     1. executable <number_of_throws> <thread_number> 
 *     2. make run N=<number_of_throws> T=<thread_number>
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 
 *     and get aggregated execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <number_o_throws>
 * Notes:  
 *    In this program OpenMP takes care of the thread creation and join, lock mechanisms
 *    and work distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "../../helpers/my_rand.h"
#include "../../helpers/timer.h"

#define NUM_ARGS 3
#define USAGE_MSG "Usage: %s <number_of_throws> <number_of_threads>\n"

/* ---------------------------- Main Function ---------------------------- */
int main(int argc, char *argv[]){
// --- 1. Argument Validation and Parsing ---
    if (argc != NUM_ARGS){
        fprintf(stderr, USAGE_MSG, argv[0]);
        return EXIT_FAILURE;
    }

    long long num_of_throws = atoll(argv[1]);
    int num_of_threads = atoi(argv[2]);

    if (num_of_throws <= 0 || num_of_threads <= 0){
        fprintf(stderr, "Error: num_throws and num_threads must be > 0.\n");
        return EXIT_FAILURE;
    }

    long long circle_darts = 0;
    double start, finish;

    GET_TIME(start);
// --- 2. Thread Creation with Private Copy of circle_darts ---
#pragma omp parallel reduction(+ : circle_darts) num_threads(num_of_threads)
    {
        unsigned thread_seed = 1234 + 17 * omp_get_thread_num();
        long long local_count = 0;

        
// --- 3. Work Distribution  ---
#pragma omp for
        for (long long i = 0; i < num_of_throws; i++){
            double x = my_drand(&thread_seed);
            double y = my_drand(&thread_seed);

// --- Monte Carlo step ---
            double sqr_dstnc = x * x + y * y;
            if (sqr_dstnc <= 1.0)
                local_count++;
        }
// --- 4. Reduction for Final Result ---
        circle_darts += local_count;
    }

    GET_TIME(finish);

    double pi = 4.0 * (double)circle_darts / (double)num_of_throws;

    printf("OpenMP Parallel Execution Time: %f\n", finish - start);
    printf("pi = %.10lf\n", pi);

    return EXIT_SUCCESS;
}