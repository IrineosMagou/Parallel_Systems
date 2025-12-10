/* Purpose:
 *   This C code implements a form of the Readers-Writers Problem 
 *   that gives priority to waiting Readers over waiting Writers. 
 *   This is often referred to as a "Writer Starvation" solution, as 
 *   a continuous stream of readers can prevent writers from ever gaining access.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../helpers/my_rand.h"
#include "../../helpers/timer.h"  

#include "globals/helpers.h"

void *Thread_task(void *rank){
    long my_rank = (long)rank;
    int i, val;
    double which_op;
    unsigned seed = my_rank + 1;
    int my_member_count = 0, my_insert_count = 0, my_delete_count = 0;
    int ops_per_thread = total_ops / thread_count;

    for (i = 0; i < ops_per_thread; i++)
    {
        which_op = my_drand(&seed); //Decide the type of operation -->Read or Write
        val = my_rand(&seed) % MAX_KEY;
        if (which_op < search_percent)
        {
            pthread_mutex_lock(&shared_resources.data_mtx);
            while (shared_resources.writing)
            {
                shared_resources.wait_read++;
                pthread_cond_wait(&shared_resources.cond, &shared_resources.data_mtx);
                shared_resources.wait_read--;
            }

            shared_resources.reading++;
            pthread_mutex_unlock(&shared_resources.data_mtx);
            Member(val);

            pthread_mutex_lock(&shared_resources.data_mtx);
            shared_resources.reading--;
            if (shared_resources.wait_wrt && (shared_resources.reading == 0) && (shared_resources.wait_read == 0))
                pthread_cond_broadcast(&shared_resources.cond0); // broadcast cause there might be more than one writer waiting.
            pthread_mutex_unlock(&shared_resources.data_mtx);
            my_member_count++;
        }
        else // cause insert or delete , is considered to be a writing operation , so the same locks
        {
            pthread_mutex_lock(&shared_resources.data_mtx);
            while (shared_resources.reading || shared_resources.writing || shared_resources.wait_read)
            {
                shared_resources.wait_wrt++;
                pthread_cond_wait(&shared_resources.cond0, &shared_resources.data_mtx);
                shared_resources.wait_wrt--;
            }

            shared_resources.writing++;
            pthread_mutex_unlock(&shared_resources.data_mtx);

            if (which_op < search_percent + insert_percent)
            {
                Insert(val);
                my_insert_count++;
            }
            else
            {
                Delete(val);
                my_delete_count++;
            }
            pthread_mutex_lock(&shared_resources.data_mtx);
            shared_resources.writing--;
            if (shared_resources.wait_wrt && (shared_resources.wait_read == 0)) // writer waiting and no reader waiting ? wake writer
                pthread_cond_broadcast(&shared_resources.cond0);
            else
                pthread_cond_broadcast(&shared_resources.cond); // no writer ? wake readers
            pthread_mutex_unlock(&shared_resources.data_mtx);
        }
    }

    pthread_mutex_lock(&count_mutex);
    member_count += my_member_count;
    insert_count += my_insert_count;
    delete_count += my_delete_count;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
}
