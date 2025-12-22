/*Purpose:  
 *   Implement a multi-threaded sorted linked list of
 *   ints with ops insert, print, member, delete, free list.
 *   Read-write locks are implemented to avoid race-conditions.
 * Compile:
 *   Makefile:
 *       make build P_SRC=priority_file.c
 * Usage:
 *   make run m=<m> n=<operations> p=<read_percentage> k=<insert_percentage> t=<num_of_threads>
 *   e.g make run m=1000 n=150000 p=0.95 k=0.03 t=2
 * Aggregate Results:
 *   Use results.sh script to run the source code for both
 *   priority approaches and compare the results.
 *   Usage:
 *     chmod +x results.sh
 *     ./results.sh <m> <operations> <read_percentage> <insert_percentage>
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../helpers/my_rand.h"
#include "../../helpers/timer.h"

#include "globals/helpers.h"

/*---------------------------- Main Function ---------------------------- */
int main(int argc, char *argv[])
{
    long i;
    int key, success, attempts;
    pthread_t *thread_handles;
    int inserts_in_main;
    unsigned seed = 1;
    double start, finish;
    char *ptr; // for strtod
    if (argc != 6) usage(argv[0]);
    
    inserts_in_main = atoi(argv[1]);
    total_ops = atoi(argv[2]);
    search_percent = strtod(argv[3], &ptr);
    insert_percent = strtod(argv[4], &ptr);
    thread_count = strtol(argv[5], NULL, 10);

    /* Try to insert inserts_in_main keys, but give up after */
    /* 2*inserts_in_main attempts.                           */
    i = attempts = 0;
    while (i < inserts_in_main && attempts < 2 * inserts_in_main)
    {
        key = my_rand(&seed) % MAX_KEY;
        success = Insert(key);
        attempts++;
        if (success)
            i++;
    }
    printf("Inserted %ld keys in empty list\n", i);
    // Print();

#ifdef OUTPUT
    printf("Before starting threads, list = \n");
    Print();
    printf("\n");
#endif

    thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&count_mutex, NULL);

    GET_TIME(start);
    for (i = 0; i < thread_count; i++)
        pthread_create(&thread_handles[i], NULL, Thread_task, (void *)i);

    for (i = 0; i < thread_count; i++)
        pthread_join(thread_handles[i], NULL);
    GET_TIME(finish);
    Print();
    printf("Elapsed time = %e seconds\n", finish - start);
    printf("Total ops = %d\n", total_ops);
    printf("member ops = %d\n", member_count);
    printf("insert ops = %d\n", insert_count);
    printf("delete ops = %d\n", delete_count);

#ifdef OUTPUT
    printf("After threads terminate, list = \n");
    Print();
    printf("\n");
#endif

    Free_list();
    pthread_mutex_destroy(&count_mutex);
    destroy_read_write_threads(&shared_resources);
    free(thread_handles);

    return 0;
} 