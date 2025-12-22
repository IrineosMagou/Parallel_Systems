#!/bin/bash
set -euo pipefail #-e:exit on error | -u:treat unset variables as an error | -o pipefail treat unset variables as an error
IFS=$'\n\t' #This defines how Bash splits words when expanding variables, reading files, etc.

# ==============================================================================
# BENCHMARK SCRIPT: MONTE CARLO PI ESTIMATION
# ==============================================================================

# --- Configuration (Global Constants) ---

# Source and File Configuration
readonly SOURCE_CODE="monte_carlo"
readonly HELPER_CODE="../../helpers/my_rand"
readonly EXECUTABLE="executable"

# Benchmarking Parameters
readonly THREAD_STEP=2
readonly MAX_THREADS=8 # Max number of threads for parallel runs
readonly REPEATS=3 # Number of runs per thread count for averaging

# Thread counts: serial (1) + parallel (2,4,6,8...)
THREAD_COUNTS=(1)
for ((t=2; t<=MAX_THREADS; t+=THREAD_STEP)); do
    THREAD_COUNTS+=($t)
done

declare -A AVG_TIMES # Store average runtimes per thread count

# --- Compilation Function ---
function compile_program {
    echo "======================================================"
    echo " --> Compiling: $SOURCE_CODE.c with $HELPER_CODE.c"
    echo "======================================================"
    
    gcc -o "$EXECUTABLE" "$SOURCE_CODE.c" "$HELPER_CODE.c" -lpthread
    
    if [ $? -ne 0 ]; then
        echo "Compilation FAILED. Aborting." >&2
        exit 1
    fi
}

# --- Benchmark Function ---
# Arguments: $1 = thread count
function run_and_average {
    local threads=$1     
    local sum_time=0.0
    local program_type
    
    if [ "$threads" -eq 1 ]; then
        program_type="Serial Execution Time"
    else
        program_type="Parallel Execution Time"
    fi
    
    echo "  Running with $threads threads (x $REPEATS repeats)..."
    
    for ((r=1; r<=REPEATS; r++)); do
        local output
        local current_time
        
        output=$(./"$EXECUTABLE" "$TOTAL_THROWS" "$threads")
        
        # Extract the time using awk, looking for the specific label
        current_time=$(echo "$output" | awk -F: "/$program_type/ {gsub(\" \",\"\",\$2); print \$2}")
        
        if [ -z "$current_time" ]; then
            echo "Error: Could not extract time for $program_type. Check output format." >&2
            current_time=0.0
        fi

        # Use 'bc' for reliable floating-point addition
        sum_time=$(echo "scale=6; $sum_time + $current_time" | bc)
    done
    
    local avg_time
    avg_time=$(echo "scale=6; $sum_time / $REPEATS" | bc)
    
    echo " Done."
    AVG_TIMES[$threads]=$avg_time 
}

# --- Main Execution Flow ---

# Input validation
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_throws>" >&2
    echo "Example: $0 100000000" >&2
    exit 1
fi

# Argument 1: Number of throws is mandatory input
readonly TOTAL_THROWS=$1

compile_program

echo "--- Starting Benchmark (N_THROWS=$TOTAL_THROWS) ---"

for threads in "${THREAD_COUNTS[@]}"; do
    run_and_average "$threads"
done

# --- Final Summary ---
echo "============== BENCHMARK SUMMARY =============="

readonly SERIAL_TIME=${AVG_TIMES[1]} 
if [ -z "$SERIAL_TIME" ]; then
    echo "Warning: Serial (1 thread) time not recorded. Cannot compute speedup."
else

    for threads in "${THREAD_COUNTS[@]}"; do
        speedup=$(echo "scale=3; $SERIAL_TIME / ${AVG_TIMES[$threads]}" | bc)
        printf "  %2s threads -> Time: %8.6f s | Speedup: %5.3fx\n" "$threads" "${AVG_TIMES[$threads]}" "$speedup"
    done
fi

# --- Cleanup ---
if [ -f "$EXECUTABLE" ]; then
    rm "$EXECUTABLE"
fi

echo "======================================================"
