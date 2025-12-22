#!/bin/bash
set -euo pipefail #-e:exit on error | -u:treat unset variables as an error | -o pipefail treat unset variables as an error
IFS=$'\n\t' #This defines how Bash splits words when expanding variables, reading files, etc.

# --- Configuration (Global Constants) ---

# Source and File Configuration
readonly EXECUTABLE="executable"
readonly MPI_SOURCE="mtx_vct_mult.c"
readonly HELPERS_SRC="helpers/helpers.c"

# Number of runs per process count for averaging
readonly RUNS_PER_NPROCS=3
readonly MAX_NPROCS=8
readonly PROC_STEP=2
readonly PROC_COUNT=(1 4 16) 

# Array to store average times for each NPROCS count
declare -a AVG_TIMES

# --- Compilation Function ---
function compile_program { 
    echo "======================================================"
    echo " --> Compiling: $MPI_SOURCE with $HELPERS_SRC"
    echo "======================================================"

# Compile the MPI program using mpicc
    mpicc -o "$EXECUTABLE" "$MPI_SOURCE" "$HELPERS_SRC" -lm

    if [ $? -ne 0 ]; then
        echo "ERROR: Compilation Failed for ${MPI_SOURCE}"
        exit 1
    fi
    echo "--------------------------------------------------------"
}

# --- Benchmark Function ---
function run_and_average {
    local processes=$1
    local sum_time=0.0
    local program_type

    if [ "$processes" -eq 1 ]; then
        program_type="Serial Execution Time"
    else
        program_type="MPI Execution Time"
    fi
    echo "Running with ${processes} processes (${RUNS_PER_NPROCS} times)..."

    for((r=1; r<RUNS_PER_NPROCS; r+=1)); do
        local output
        local current_time

        # Extract the time using awk, looking for the print of the C source code.
        output=$(mpiexec -n "$processes" ./"$EXECUTABLE" "$MATRIX_DIMENSIONS")
        current_time=$(echo "$output" | awk -F: "/$program_type/ {gsub(\" \",\"\",\$2); print \$2}")
        
        if [[ -z "$current_time" ]]; then
            echo "Warning: Could not extract time on run ${program_type} for processes=${processes}. Skipping this run."
            continue
        fi

        # Use 'bc' for reliable floating-point addition
        sum_time=$(echo "scale=6; $sum_time + $current_time" | bc)
    done

    local avg_time
    avg_time=$(echo "scale=6; $sum_time / $RUNS_PER_NPROCS" | bc)

    echo " Done. "
    AVG_TIMES[$processes]=$avg_time
}

# --- Main Execution Flow ---


# --- Input Validation ---
if [ $# -ne 1 ]; then
    echo "Usage: $0 <matrix_dimensions>"
    echo "Example: $0 8000"
    exit 1
fi

# Argument 1: Number of throws is mandatory input
MATRIX_DIMENSIONS=$1

compile_program

echo "--- Starting Benchmark (N_THROWS=$MATRIX_DIMENSIONS) ---"

for proc in "${PROC_COUNT[@]}"; do
    run_and_average "$proc"
done

# --- Final Summary ---
echo "============== BENCHMARK SUMMARY =============="
readonly SERIAL_TIME=${AVG_TIMES[1]} 
if [ -z "$SERIAL_TIME" ]; then
    echo "Warning: Serial (1 thread) time not recorded. Cannot compute speedup."
else

    for process in "${PROC_COUNT[@]}"; do
        speedup=$(echo "scale=3; $SERIAL_TIME / ${AVG_TIMES[$process]}" | bc)
        printf "  %2s process -> Time: %8.6f s | Speedup: %5.3fx\n" "$process" "${AVG_TIMES[$process]}" "$speedup"
    done
fi

# --- Cleanup ---
if [ -f "$EXECUTABLE" ]; then
    rm "$EXECUTABLE"
fi

echo "==============================================="