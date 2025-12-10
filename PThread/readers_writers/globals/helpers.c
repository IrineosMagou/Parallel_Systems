/* Purpose:
 *     Functions that help with the reading and writing(insert-delete)
 *      operations of the linked list. Also functions for the shared 
 *      structure that the threads have to lock/unlock and count the
 *      queue.
 */



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "helpers.h"


list_node_s *head = NULL;

/* -------------------- Usage / Help -------------------- */
void Usage(char *prog_name){
    fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
    exit(0);
} 
 /*-----------------------------------------------------------------*/
// void Get_input(){
//     printf("How many keys should be inserted in the main thread?\n");
//     scanf("%d", inserts_in_main);
//     printf("How many ops total should be executed?\n");
//     scanf("%d", &total_ops);
//     printf("Percent of ops that should be searches? (between 0 and 1)\n");
//     scanf("%lf", &search_percent);
//     printf("Percent of ops that should be inserts? (between 0 and 1)\n");
//     scanf("%lf", &insert_percent);
//     delete_percent = 1.0 - (search_percent + insert_percent);
// } 

/* ----------- Initialize Shared Structure ------------- */
int init_read_write_threads(read_write_data *rw_struct) {
    int result = 0;
// 1. Initialize the Mutex
    if (pthread_mutex_init(&rw_struct->data_mtx, NULL) != 0) {
        perror("Mutex init failed");
        result = -1;
    }
// 2. Initialize Condition Variables (for reading and writing)
    if (pthread_cond_init(&rw_struct->cond, NULL) != 0) {
        perror("Read condition init failed");
        // Add cleanup code for the mutex here in a production system
        result = -1;
    }
    if (pthread_cond_init(&rw_struct->cond0, NULL) != 0) {
        perror("Write condition init failed");
        // Add cleanup code for the mutex and cond here
        result = -1;
    }
// 3. Initialize the counters
    rw_struct->wait_read = 0;
    rw_struct->reading = 0;
    rw_struct->wait_wrt = 0;
    rw_struct->writing = 0;
    
    return result;
}

/* ------------------ Cleanup Shared Structure ------------------ */
void destroy_read_write_threads(read_write_data *rw_struct) {
    pthread_mutex_destroy(&rw_struct->data_mtx);
    pthread_cond_destroy(&rw_struct->cond);
    pthread_cond_destroy(&rw_struct->cond0);
}

/*---------------------- List Oeprations ---------------------- */

/*------------------- Insert Node Operation ------------------- */
/* Insert value in correct numerical location into list */
/* If value is not in list, return 1, else return 0 */
int Insert(int value){
    list_node_s *curr = head;
    list_node_s *pred = NULL;
    list_node_s *temp;
    int rv = 1;

    while (curr != NULL && curr->data < value){
        pred = curr;
        curr = curr->next;
    }

    if (curr == NULL || curr->data > value){
        temp = malloc(sizeof(list_node_s));
        temp->data = value;
        temp->next = curr;
        if (pred == NULL)
            head = temp;
        else
            pred->next = temp;
    }
    else{ /* value in list */
        not_insert++;
        rv = 0;
    }

    return rv;
} 
/*------------------ Print Operation ------------------*/
void Print(void){
    list_node_s *temp;
    int i = 0;

    temp = head;

    while (temp != (list_node_s *)NULL){
        i++;
        temp = temp->next;
    }
    printf("num of nodes %d\n", i); // to make sure that all the operations are correctlty done
    printf("The numbers of insertion is %d and of deletes %d\n", insert_count - not_insert, delete_count - not_deleted);
} 

/*-----------------------------------------------------------------*/
int Member(int value){
    list_node_s *temp;
    temp = head;

    while (temp != NULL && temp->data < value)
        temp = temp->next;

    if (temp == NULL || temp->data > value){
#ifdef DEBUG
        printf("%d is not in the list\n", value);
#endif
        return 0;
    }
    else{
#ifdef DEBUG
        printf("%d is in the list\n", value);
#endif
        return 1;
    }
} 
/*--------------------- Delete Node Operation --------------------- */
/* Deletes value from list */
/* If value is in list, return 1, else return 0 */
int Delete(int value){
    list_node_s *curr = head;
    list_node_s *pred = NULL;
    int rv = 1;

    while (curr != NULL && curr->data < value){
        pred = curr;
        curr = curr->next;
    }

    if (curr != NULL && curr->data == value){
        if (pred == NULL){ /* first element in list */
            head = curr->next;
#ifdef DEBUG
            printf("Freeing %d\n", value);
#endif
            free(curr);
        }
        else{
            pred->next = curr->next;
#ifdef DEBUG
            printf("Freeing %d\n", value);
#endif
            free(curr);
        }
    }
    else{ /* Not in list */
        not_deleted++;
        rv = 0;
    }

    return rv;
} /* Delete */
/*------------------ Free-List Operation --------------------*/
void Free_list(void){
    list_node_s *current;
    list_node_s *following;

    if (Is_empty())
        return;
    current = head;
    following = current->next;
    while (following != NULL){
#ifdef DEBUG
        printf("Freeing %d\n", current->data);
#endif
        free(current);
        current = following;
        following = current->next;
    }
#ifdef DEBUG
    printf("Freeing %d\n", current->data);
#endif
    free(current);
} 
/*------------------- Check If List Is Empty ------------------- */
int Is_empty(void){
    if (head == NULL)
        return 1;
    else
        return 0;
} 
