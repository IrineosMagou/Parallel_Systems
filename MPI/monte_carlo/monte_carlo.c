/* Purpose:  
 *     Computes pi using the Monte Carlo approach. The total throws are
 *     distributed across the processes equally and each process calculates 
 *     it's own circle hits and then these hits are collected back
 *     to the parent process, summed up and calculate π.
 * Output:
 *     Parallel execution times
 *     Estimation of pi
 * Compile:  
 *     1. Terminal Command for Local run:
 *              mpicc -o executable monte_carlo.c ../../helpers/my_rand.c 
 *     2. Makefile:
 *              make
 * Usage:
 *     1. mpiexec -n <num_of_processes> executable <number_of_throws> 
 *     2. make run p=<num_of_processes> n=<number_of_throws> 
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 
 *     and get aggregated execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <number_of_throws>
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../../helpers/my_rand.h"
#include "../../helpers/timer.h"

double start, finish;
unsigned seed = 1, x;

/* ---------------------------- Main Function ---------------------------- */
int main(int argc, char *argv[]){
// --- 1. Variable Declaration and MPI Initialization ---
    long int num_of_throws, total_circle_darts, local_circle_darts = 0;
    int my_id, nproc;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

// --- 2. Argument Validation and Parsing ---
    if (my_id == 0){
        if (argc != 2) {
            fprintf(stderr, "Usage: %s <number_of_throws>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);  // Ensure all processes terminate
        }
        num_of_throws = atof(argv[1]);
    }
// --- 3. Broadcast the Number of throws ---
    MPI_Bcast(&num_of_throws, 1, MPI_LONG, 0, MPI_COMM_WORLD);

// --- 4. Seed the Random Number Generator ---
    seed = 1234 + my_id; 
    unsigned x = my_rand(&seed);
    
    if(my_id == 0) GET_TIME(start);

// --- 5. The core Monte Carlo simulation loop ---
    for (int i = my_id; i < num_of_throws; i += nproc){
        double k = my_drand(&x);
        double y = my_drand(&x);
        double sqr_dstnc = k * k + y * y;
        if (sqr_dstnc <= 1)
            local_circle_darts++;
    }

// --- 6. Sum-up/Reduce all Circle Hits ---
    MPI_Reduce(&local_circle_darts, &total_circle_darts, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

// --- 7. Root Process Computes π ---
    if (my_id == 0){
        double pi = 4 * total_circle_darts / ((double)num_of_throws);
        GET_TIME(finish)
        printf("MPI Execution Time: %f\n", finish - start);
        printf("pi = %.2f\n", pi);
    }
	MPI_Finalize();
}