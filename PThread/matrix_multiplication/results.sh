#!/bin/bash
set -euo pipefail #-e:exit on error | -u:treat unset variables as an error | -o pipefail treat unset variables as an error
IFS=$'\n\t' #This defines how Bash splits words when expanding variables, reading files, etc.

# ==============================================================================
# BENCHMARK SCRIPT: MATRIX MULTIPLICATION WITH-WITHOUT PADDING
# ==============================================================================

# --- Configuration (Global Constants) ---
# Programs to benchmark (list of C source files)
readonly PROGRAMS=("false_sharing.c" "padding_matrix.c" "local_matrix.c") 
readonly EXECUTABLE="./mm_bench" # Single, consistent executable name
readonly HELPERS_SRC="helpers/matrix_mul_helpers.c"
readonly THREADS_START=2
readonly THREADS=(2 4 8) #Runs for 2, 4, 8 threads (3 iterations)
readonly RUNS_PER_THREAD=3

# --- Helper Function to Run Benchmarks ---
# Arguments: $1 = source file, $2 = rowsA, $3 = colsA/rowsB, $4 = colsB
run_benchmark() {
    # Local variables for function arguments and temporary storage (lowercase)
    local source=$1
    local m=$2
    local n=$3
    local p=$4
    local program_name
    program_name=$(basename "$source" .c) # e.g., "false_sharing"

    echo "======================================================"
    echo " --> Compiling: $source"
    echo "======================================================"
    
    # Compile the source file, checking for errors
    # Using global constants EXECUTABLE and HELPERS_SRC
    gcc -o "$EXECUTABLE" "$source" "$HELPERS_SRC" -lpthread
    if [ $? -ne 0 ]; then
        echo "--> Compilation of $source FAILED. Skipping." >&2
        return 1
    fi
    
    local total_time=0
    local threads_time=()
    local num_threads=$THREADS_START # Using global constant THREADS_START

    for num_threads in "${THREADS[@]}"; do # Using global constant THREADS_COUNT
        local thread_sum=0
        
        echo "  Running with $num_threads threads..."
        
        for ((j=1; j<=$RUNS_PER_THREAD; j++)); do # Using global constant RUNS_PER_THREAD
            # Run the executable, capturing stdout
            local output
            output=$("$EXECUTABLE" "$m" "$n" "$p" "$num_threads")
            
            # Use awk to reliably extract the time
            local parallel_time
            parallel_time=$(echo "$output" | awk '/Parallel Execution Time/{printf "%.6f", $4}')
            
            # Use 'bc' for floating-point arithmetic aggregation
            thread_sum=$(echo "scale=6; $thread_sum + $parallel_time" | bc)
            
            echo -n "." # Progress dot
        done
        
        # Calculate and store the average time for this thread count
        local avg_time
        avg_time=$(echo "scale=6; $thread_sum / $RUNS_PER_THREAD" | bc)
        threads_time+=("$avg_time")
        total_time=$(echo "scale=6; $total_time + $thread_sum" | bc)
        
        echo " Done. Avg time: $avg_time s"
    done
    
    # Calculate overall average time
    #${#THREADS[@]} size of array
    local total_runs=$((${#THREADS[@]} * RUNS_PER_THREAD))
    local overall_avg
    overall_avg=$(echo "scale=6; $total_time / $total_runs" | bc)

    echo "--- Results for $program_name (M=$m, N=$n, P=$p) ---"
    echo "Thread Averages: ${threads_time[@]}"
    echo "Overall Average: $overall_avg s"
    
    # Store results in the global associative array
    program_times["$program_name"]="${threads_time[*]}"
}

# --- Main Script Execution ---

# Input validation
if [ $# -ne 3 ]; then
    echo "Usage: $0 <rowsA> <columnsA_rowsB> <columnsB>" >&2
    exit 1
fi

declare -A program_times # Global array to store the different times of each program

# Loop through all defined programs
for src_file in "${PROGRAMS[@]}"; do
    run_benchmark "$src_file" "$1" "$2" "$3"
done

echo "======================================================"
echo "--> ALL BENCHMARKS COMPLETE"
echo "Results (Thread Averages by Program):"
for program in "${!program_times[@]}"; do
    echo "$program: " 
    echo "${program_times[$program]}"
done
echo "======================================================"

# --- Cleanup ---
if [ -f "$EXECUTABLE" ]; then
    rm "$EXECUTABLE"
fi