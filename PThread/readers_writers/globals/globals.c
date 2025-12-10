/* Purpose:
*   Definitions only for the shared variables. By defining the 
*   variables here, we avoid multiple-definition and redefine
*   erros. All the source files that need these variable, can 
*   access them from here.
*/ 

#include <pthread.h>
#include "helpers.h" // Get all the declarations

// 1. Define and Initialize the Shared Counters
int member_count = 0;
int delete_count = 0;
int not_deleted = 0; 
int not_insert = 0;
int insert_count = 0;
int thread_count = 0;
int total_ops = 0;
double insert_percent = 0.0;
double search_percent = 0.0;

// 2. Define and Initialize Shared Synchronization Resources
pthread_mutex_t count_mutex; // Will be initialized by function call in main.c
read_write_data shared_resources; // Will be initialized by function call in main.c
