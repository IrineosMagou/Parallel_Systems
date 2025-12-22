
/* Purpose:
 *     The program performs a parallel matrix–vector multiplication using MPI.
 *     The matrix is distributed across processes arranged in a 2D square grid,
 *     and the vector is distributed accordingly. Each process computes a partial 
 *     result, which is then combined to form the final vector.
 * Output:
 *     MPI execution time
 *     Result Validation: Correct ot Wrong Vector Product
 * Compile:  
 *     1. Terminal Command for Local run:
 *              mpicc -o executable mtx_vct_mult.c helpers/helpers.c -lm 
 *     2. Makefile:
 *              make
 * Usage:
 *     1. mpiexec -n <num_of_processes> executable <matrix_dimensions> 
 *     2. make run p=<num_of_processes> n=<matrix_dimensions> 
 * Aggregate Results:
 *     Use results.sh script to run the source code multiple times for threads 1-8 
 *     and get aggregated execution time results
 *     Usage:
 *         chmod +x results.sh
 *         ./results.sh <matrix_dimensions>
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>

#include "helpers/helpers.h"
#include "../../helpers/timer.h"

double start, finish;
/* ---------------------------- Main Function ---------------------------- */
int main(int argc, char **argv) {

    // MPI variables
    int rank, size;        // rank of current process and no. of processes
    MPI_Comm comm = MPI_COMM_WORLD;
    
    //Dimensions
    int global_mtx_dimension, local_dim; // size of grid
    int proc_grid; // how the processes are distributed on the matrix
    
    //Data Structures
    double **local, *local_v, *local_p;
    double *global_vector_v, **global, *global_vector_p;//this are only for rank_0

    // MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    proc_grid = (int)(sqrt((double)size)+0.5);
    if (proc_grid * proc_grid != size) {
        if (rank == 0)
            fprintf(stderr, "Number of processes must be a perfect square\n");
        MPI_Abort(comm, EXIT_FAILURE);
    }

/* Rank 0 reads input and initializes data */
    if (rank == 0){
// --- 1. Parse and Check CL Arguments ---
        if(argc != 2) usage(argv[0]);

        global_mtx_dimension = atoi(argv[1]);
        if ((global_mtx_dimension % proc_grid) != 0){
            fprintf(stderr, "Matrix dimension must be divisible by sqrt(num_processes)\n");
            MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
        }

        local_dim = global_mtx_dimension / proc_grid;
    
// --- 2. Allocate and Initialize Global Data ---
        malloc2double(&global, global_mtx_dimension);
        global_vector_v = (double*)malloc(global_mtx_dimension*sizeof(double));
        global_vector_p = (double*)malloc(global_mtx_dimension*sizeof(double));
        gen_mtx_vct(global , global_vector_v , global_mtx_dimension);    
        
        //In case of only one process provided
        if(size == 1){
                GET_TIME(start);
                double *serial_p = malloc((global_mtx_dimension)*sizeof(double));
                mtx_vct_mul(*global, global_vector_v, serial_p, global_mtx_dimension);
                GET_TIME(finish);
                printf("Serial Execution Time: %f\n", finish - start);
                free(serial_p);

                MPI_Finalize();
                return EXIT_SUCCESS;
        }
    }

// --- 3. Broadcast and Allocate Local Data ---
    MPI_Bcast(&local_dim , 1 , MPI_INT , 0 , comm);

    malloc2double(&local, local_dim);
    local_v = malloc((local_dim)*sizeof(double));
    local_p = malloc((local_dim)*sizeof(double));

// ---4. Global Matrix Distribution ---
    // Create derived datatype
    int sizes[2]    = {local_dim*proc_grid, local_dim*proc_grid}; // global size
    int subsizes[2] = {local_dim, local_dim}; 
    int starts[2]   = {0,0}; 

    MPI_Datatype type, subarrtype;
    MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_DOUBLE, &type);
    MPI_Type_create_resized(type, 0, local_dim*sizeof(double), &subarrtype);
    MPI_Type_commit(&subarrtype);

    double *globalptr=NULL;
    if (rank == 0) globalptr = &(global[0][0]);

// --- 5. Fill the array of displacements and Distribute the Matrix ---
    int sendcounts[size];
    int displs[size];

    if (rank == 0){
        for (int i=0; i<size; i++) 
            sendcounts[i] = 1;

        int disp = 0;
        for (int i=0; i<proc_grid; i++) {
            for (int j=0; j<proc_grid; j++) {
                displs[i*proc_grid+j] = disp ;
                disp += 1 ; 
            }
            disp += (local_dim - 1) * proc_grid;    
        }     
    }

    MPI_Scatterv(globalptr, sendcounts, displs , subarrtype, &(local[0][0]),
                            local_dim*local_dim, MPI_DOUBLE,0, comm);

// --- 6. Fill the array of displacements and Distribute the Vector.
    int displs_vaa[size] , sendcounts_vaa[size];
    if (rank == 0) {
        for (int i=0; i<proc_grid; i++) {
            for (int j=0; j<proc_grid; j++) {
                 displs_vaa[i*proc_grid + j] = j*(local_dim);
                sendcounts_vaa[i*proc_grid + j] = local_dim ;
            }
        }     
        GET_TIME(start);     
    }

    MPI_Scatterv(global_vector_v, sendcounts_vaa, displs_vaa, MPI_DOUBLE, &(local_v[0]),
                 local_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);

// --- 7. Local Computation ---
    mtx_vct_mul(*local , local_v , local_p , local_dim );

// --- 8. Row-wise Reduction ---
    int reduc_size = local_dim / proc_grid ;
    double reduction_result[reduc_size];
    MPI_Comm row_comm;

    MPI_Comm_split(MPI_COMM_WORLD, rank / proc_grid, rank, &row_comm);
    int counts[reduc_size] ;
// The amount of elements that will be received from each process after the reduction
    for(int i = 0 ; i < sqrt(size) ; i++)
      counts[i] = reduc_size ;

    MPI_Reduce_scatter_block(local_p, reduction_result, *counts,
                             MPI_DOUBLE, MPI_SUM, row_comm);
                             
    MPI_Gather(&reduction_result, reduc_size, MPI_DOUBLE, global_vector_p, 
                            reduc_size, MPI_DOUBLE, 0, comm);

// --- 9. Rank 0: Result Validation and Free Memory ---                         
    if (rank == 0) {
        GET_TIME(finish);
        printf("MPI Execution Time: %f\n", finish - start);
//Uncomment for result validation
        // GET_TIME(start);
        // double *serial_p = malloc((global_mtx_dimension)*sizeof(double));
        // mtx_vct_mul(*global, global_vector_v, serial_p, global_mtx_dimension);
        // GET_TIME(finish);
        // printf("Serial Execution time: %f\n", finish - start);
        // if(results_validation(global_mtx_dimension, serial_p, global_vector_p) == EXIT_SUCCESS)
        //     printf("Correct Product\n");
        // else
        //     printf("Wrong Product\n");
        // free(serial_p);

        free2double(&global);
        free(global_vector_v);
        free(global_vector_p);
    }

// --- 10. Free Allocated Memory ---
    free(local_v);
    free(local_p);

    free2double(&local);

    MPI_Type_free(&type);
    MPI_Type_free(&subarrtype);
    MPI_Comm_free(&row_comm);
    
    MPI_Finalize();
    return EXIT_SUCCESS;
}

