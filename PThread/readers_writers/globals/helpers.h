/* The header file with the declarations of the variables, 
 * data structures and function names that all the source files use.
 */


#ifndef _HELPERS_H
#define _HELPERS_H

/* Random ints are less than MAX_KEY */
#define MAX_KEY 100000000;

/* ---------------- Extern Variables Declaration ---------------- */
extern int member_count;
extern int delete_count;
extern int not_deleted; 
extern int not_insert;
extern int insert_count;
extern int thread_count;
extern int total_ops;
extern double insert_percent;
extern double search_percent;
extern double delete_percent;
extern pthread_mutex_t count_mutex;


// ---------- Structure for Threads -------------
typedef struct{
    pthread_mutex_t data_mtx;
    pthread_cond_t cond;  // read
    pthread_cond_t cond0; // write
    int wait_read;
    int reading;
    int wait_wrt;
    int writing;
}read_write_data;

extern read_write_data shared_resources;   

/* ---------------- Helper Functions For Threads --------------- */
void usage(char *prog_name);
void get_input(int *inserts_in_main_p);
int init_read_write_threads(read_write_data *rw_struct);
void destroy_read_write_threads(read_write_data *rw_struct);
void *Thread_task(void *rank);


/* -------------- Struct for list nodes -------------- */
typedef struct node_tag{
    int data;
    struct node_tag *next;
}list_node_s;

extern list_node_s *head;

/*----------------- List operations ----------------- */
int Insert(int value);
void Print(void);
int Member(int value);
int Delete(int value);
void Free_list(void);
int Is_empty(void);

#endif
