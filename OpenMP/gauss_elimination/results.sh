#!/bin/bash
set -euo pipefail   # Exit on error, unset variables are errors, fail on pipe errors
IFS=$'\n\t'

# ==============================================================================
# BENCHMARK SCRIPT: GAUSSIAN ELIMINATION
# ==============================================================================

# --- Configuration (Global Constants) ---
readonly MATRIX_SIZE=$1

# Source and executable configuration
readonly SOURCE_CODE="gauss_main.c"
readonly GAUSS_SRC="gauss_elim.c"
readonly HELPERS_GAUSS_SRC="helpers/helpers.c"
readonly HELPERS_SRC="../../helpers/my_rand.c"
readonly CFLAGS="-fopenmp"
readonly EXECUTABLE="executable"

# Benchmarking parameters
readonly REPEATS=3
readonly THREAD_STEP=2
readonly MAX_THREADS=8

# Thread counts: serial (1) + parallel (2,4,6,8...)
THREAD_COUNTS=(1)
for ((t=2; t<=MAX_THREADS; t+=THREAD_STEP)); do
    THREAD_COUNTS+=($t)
done

declare -A avg_times   # Store average runtimes per thread count

# --- Compilation Function ---
function compile_program {
    echo "======================================================"
    echo " --> Compiling: $SOURCE_CODE"
    echo "======================================================"

    gcc -o "$EXECUTABLE" "$SOURCE_CODE" "$GAUSS_SRC" "$HELPERS_GAUSS_SRC" "$HELPERS_SRC" "$CFLAGS"

    if [ $? -ne 0 ]; then
        echo "Compilation FAILED. Aborting." >&2
        exit 1
    fi
}

# --- Benchmark Function ---
# Arguments: $1 = number of threads
function run_and_average {
    local threads=$1
    local sum_time=0.0
    local program_type

    if [ "$threads" -eq 1 ]; then      
        program_type="Serial Gauss Elimination"
    else    
        program_type="Parallel Gauss Elimination"
    fi

    echo "  Running with $threads thread(s) (x $REPEATS repeats)..."

    for ((r=1; r<=REPEATS; r++)); do
        local output
        local current_time

        output=$(./"$EXECUTABLE" "$MATRIX_SIZE" "$threads")
        # Extract the time using awk, looking for the specific label
        current_time=$(echo "$output" | awk -F: "/$program_type/ {gsub(\" \",\"\",\$2); print \$2}")

        if [ -z "$current_time" ]; then
            echo "Error: Failed to extract timing for $program_type" >&2
            current_time=0.0
        fi
        sum_time=$(echo "scale=6; $sum_time + $current_time" | bc)
    done
    
    local avg_time
    avg_time=$(echo "scale=6; $sum_time / $REPEATS" | bc)

    echo " Done."
    avg_times[$threads]=$avg_time # Store result in global array
    #echo "  -> Average Time for $threads threads: $avg_time seconds"

}

# --- Main Execution Flow ---
# --- Input Validation ---
if [ $# -ne 1 ]; then
    echo "Usage: $0 <size_of_linear_system>" >&2
    echo "Example: $0 1024" >&2
    exit 1
fi
compile_program

echo "--- Starting Benchmark (Matrix Size = $MATRIX_SIZE) ---"

for threads in "${THREAD_COUNTS[@]}"; do
    run_and_average "$threads"
done

# --- Results & Speedup ---
echo "======================================================"
echo " --> BENCHMARK RESULTS"
echo "======================================================"

readonly SERIAL_TIME=${avg_times[1]}

if [ -z "$SERIAL_TIME" ]; then
    echo "Warning: Serial runtime not recorded. Speedup unavailable."
else
    echo "Serial Time (1 thread): $SERIAL_TIME seconds"
    echo "Speedup relative to Serial Execution:"

    for threads in "${THREAD_COUNTS[@]}"; do
        speedup=$(echo "scale=3; $SERIAL_TIME / ${avg_times[$threads]}" | bc)
        printf "  %2d threads -> Time: %8.6f s | Speedup: %5.3fx\n" \
               "$threads" "${avg_times[$threads]}" "$speedup"
    done
fi

# --- Cleanup ---
rm -f "$EXECUTABLE"

echo "======================================================"
