#!/bin/bash
set -euo pipefail #-e:exit on error | -u:treat unset variables as an error | -o pipefail treat unset variables as an error
IFS=$'\n\t' #This defines how Bash splits words when expanding variables, reading files, etc.

# ==============================================================================
# BENCHMARK SCRIPT: MONTE CARLO PI ESTIMATION
# ==============================================================================

# --- Configuration (Global Constants) ---
# Argument 1: Number of throws is mandatory input
readonly NUM_THROWS=$1

# Source and File Configuration
readonly SOURCE_CODE="monte_carlo"
readonly HELPER_CODE="../../helpers/my_rand"
readonly EXECUTABLE="executable"

# Benchmarking Parameters
readonly THREAD_STEP=2
readonly MAX_THREADS=8 # Max number of threads for parallel runs
readonly REPEATS=3 # Number of runs per thread count for averaging

# Array of thread counts: Starts with 1 (serial), then 2, 4, 6, 8...
THREAD_COUNTS=(1)
for ((t=2; t<=MAX_THREADS; t+=THREAD_STEP)); do
    THREAD_COUNTS+=($t)
done

declare -A avg_times # Global associative array to store results

# --- Compilation Function ---

function compile_program {
    echo "======================================================"
    echo " --> Compiling: $SOURCE_CODE.c with $HELPER_CODE.c"
    echo "======================================================"
    
    # Compile the Monte Carlo program
    # Using global constants
    gcc -o "$EXECUTABLE" "$SOURCE_CODE.c" "$HELPER_CODE.c" -fopenmp
    
    if [ $? -ne 0 ]; then
        echo "Compilation FAILED. Aborting." >&2
        exit 1
    fi
}

# --- Core Benchmarking Function ---
# Arguments: $1 = thread count
function run_and_average {
    local threads=$1      # Local argument
    local sum_time=0.0
    local program_type
    program_type="OpenMP Parallel Execution Time"

    echo "  Running with $threads threads (x $REPEATS repeats)..."
    
    for ((r=1; r<=REPEATS; r++)); do
        local output
        local current_time
        
        # Run the executable, passing throws and thread count
        # Using global constant EXECUTABLE and NUM_THROWS
        output=$(./"$EXECUTABLE" "$NUM_THROWS" "$threads")
        
        # Extract the time using awk, looking for the specific label
        current_time=$(echo "$output" | awk -F: "/$program_type/ {gsub(\" \",\"\",\$2); print \$2}")
        
        # Robust check to prevent bc errors
        if [ -z "$current_time" ]; then
            echo "Error: Could not extract time for $program_type. Check output format." >&2
            current_time=0.0
        fi

        # Use 'bc' for reliable floating-point addition
        sum_time=$(echo "scale=6; $sum_time + $current_time" | bc)
        echo -n "." # Progress dot
    done
    
    # Calculate average time using 'bc'
    local avg_time
    avg_time=$(echo "scale=6; $sum_time / $REPEATS" | bc)
    
    echo " Done."
    avg_times[$threads]=$avg_time # Store result in global array
    #echo "  -> Average Time for $threads threads: $avg_time seconds"
}

# --- Main Execution Flow ---

# Input validation
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_throws>" >&2
    echo "Example: $0 100000000" >&2
    exit 1
fi
compile_program

echo "--- Starting Benchmark (N_THROWS=$NUM_THROWS) ---"

for threads in "${THREAD_COUNTS[@]}"; do
    run_and_average "$threads"
done

# --- Speedup Calculation & Final Output ---

echo "======================================================"
echo " --> BENCHMARK RESULTS"
echo "======================================================"

# Check if serial time (key 1) exists before calculating speedup
# Using uppercase for SERIAL_TIME as it is a critical calculation result derived from a global store.
readonly SERIAL_TIME=${avg_times[1]} 
if [ -z "$SERIAL_TIME" ]; then
    echo "Warning: Serial (1 thread) time not recorded. Cannot compute speedup."
else
    echo "Serial Time (1 thread): $SERIAL_TIME seconds"
    echo "Speedup relative to Serial Time:"

    for threads in "${THREAD_COUNTS[@]}"; do
        # Calculate speedup using 'bc'
        # Local variable
        speedup=$(echo "scale=3; $SERIAL_TIME / ${avg_times[$threads]}" | bc)
        
        printf "  %2s threads -> Time: %8.6f s | Speedup: %5.3fx\n" "$threads" "${avg_times[$threads]}" "$speedup"
    done
fi

# --- Cleanup ---
if [ -f "$EXECUTABLE" ]; then
    rm "$EXECUTABLE"
fi

echo "======================================================"