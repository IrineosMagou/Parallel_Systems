/* Purpose:  
 *     Computes pi using the Monte Carlo approach.
 * Output:
 *     Serial and Parallel execution times
 *     Estimation of pi
 * Compile:  
 *     1. Terminal Command:
 *              gcc -Wall -o executable monte_carlo.c ../../helpers?/my_rand.c -lpthread
 *     2. Makefile:
 *              make
 * Usage:
 *     1. executable <number_of_throws> <thread_number> 
 *     2. make run N=<number_of_throws> T=<thread_number>
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 and get aggregated 
 *     execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <number_o_throws>
 * Notes:  
 *    This program doesn't need any lock mechanism because each thread keeps track of
 *    its circle hits in its own local memory space. Then these results are summed up
 *    during thread joining.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../helpers/my_rand.h" 
#include "../../helpers/timer.h"  

#define NUM_ARGS 3
#define USAGE_MSG "Usage: %s <number_of_throws> <number_of_threads>\n"

/* Structure to pass throws count IN and receive hits count OUT */
typedef struct {
    long long int my_num_of_throws;
    long long int my_hits_in_circle; // <-- Hold the thread's local result
    int my_rank;
} ThreadData;

/* ------------------ Global Variables & Function Prototypes ------------------ */
void *calculate_throws_task(void *data);
void create_threads(pthread_t *thread_handles, int num_of_threads, int num_of_throws, ThreadData *thread_data);
long long int join_threads(pthread_t *thread_handles, int num_of_threads, ThreadData *thread_data);
unsigned seed = 1;

/* -------------------------- Thread Creation --------------------------- */
void create_threads(pthread_t *thread_handles, int num_of_threads, int num_of_throws, ThreadData *thread_data){
    double start, finish;
    long long int throws_per_thread = num_of_throws / num_of_threads;
    long long int remaining_throws = num_of_throws % num_of_threads;
    
    for(long i = 0; i < num_of_threads; i++){
// Distribute throws evenly, with the remainder going to the first threads
        thread_data[i].my_num_of_throws = throws_per_thread;
        if (i < remaining_throws) {
            thread_data[i].my_num_of_throws++;
        }
        
        thread_data[i].my_rank = i;

        int status = pthread_create(&thread_handles[i], NULL, calculate_throws_task, &thread_data[i]);
        if (status != 0) {
            fprintf(stderr, "Error creating thread %ld. Exiting.\n", i);
// Handle cleanup and exit if thread creation fails
            break; 
        }
    }
}
/* -------------------------- Thread Joining --------------------------- */
long long int join_threads(pthread_t *thread_handles, int num_of_threads, ThreadData *thread_data){
    long long int total_hits = 0;
    for(long i = 0; i < num_of_threads; i++) {
        pthread_join(thread_handles[i], NULL);
        // Sum up the local results stored in the thread_data structure
        total_hits += thread_data[i].my_hits_in_circle;
    }
    return total_hits;   
}
/* ---------------------------- Thread Task----------------------------- */
void *calculate_throws_task(void *data){
    ThreadData *my_data = (ThreadData *)data;
    long my_throws = my_data->my_num_of_throws;
// The my_rand() function takes a pointer to the seed, so we need a local variable
    unsigned thread_seed = seed*(my_data->my_rank + 1); 
    long my_circle_darts = 0; // Local counter for hits
    
// --- The core Monte Carlo simulation loop ---
    for (long throw = 0; throw < my_throws; throw++){

        double k = my_drand(&thread_seed);
        double y = my_drand(&thread_seed);
        double sqr_dstnc = k * k + y * y;
        
        if (sqr_dstnc <= 1.0) { // Check if distance squared is less than or equal to 1
          my_circle_darts++;
        }
    }
// Store the result locally 
    my_data->my_hits_in_circle = my_circle_darts;

    return NULL;
}

/* ---------------------------- Main Function ---------------------------- */
int main(int argc, char *argv[]){
// --- 1. Argument Validation and Parsing ---
    if(argc != NUM_ARGS) {
        fprintf(stderr, USAGE_MSG, argv[0]); 
        return EXIT_FAILURE; 
    }

    long long int num_of_throws = strtoll(argv[1], NULL, 10);
    int num_of_threads = atoi(argv[2]);
    
    if (num_of_throws <= 0 || num_of_threads <= 0) {
        fprintf(stderr, "Number of throws and threads must be positive.\n");
        return EXIT_FAILURE;
    }

// --- 2. Resource Allocation ---
    pthread_t *thread_handles = malloc(num_of_threads * sizeof(pthread_t));
    ThreadData *thread_data = malloc(num_of_threads * sizeof(ThreadData)); 
    
    if (thread_handles == NULL || thread_data == NULL) {
        perror("malloc failed");
        free(thread_handles);
        free(thread_data);
        return EXIT_FAILURE;
    }
    double start, finish;
    
// --- 3. Work Distribution and Thread Creation ---
// --- 4. Thread Joining and Sequential Aggregation (Reduction) ---
    GET_TIME(start)
    create_threads(thread_handles, num_of_threads, num_of_throws, thread_data);
    long long int total_hits = join_threads(thread_handles, num_of_threads, thread_data);
    GET_TIME(finish)
    
// --- 5. Final Calculation and Cleanup/De-Allocation ---
    double pi = 4.0 * ((double)total_hits / num_of_throws);
    
    if(num_of_threads == 1)
        printf("Serial Execution Time: %f\n", finish - start);
    else
        printf("Parallel Execution Time: %f\n", finish - start);
    printf("Estimated Pi: %.6f\n", pi);
    
    free(thread_handles);
    free(thread_data);
    
    return EXIT_SUCCESS;
}