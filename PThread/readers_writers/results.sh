#!/bin/bash
set -euo pipefail #-e:exit on error | -u:treat unset variables as an error | -o pipefail treat unset variables as an error
IFS=$'\n\t' #This defines how Bash splits words when expanding variables, reading files, etc.

# ==============================================================================
# CONFIGURATION
# ==============================================================================


# Global Configuration Constants (UPPERCASE)
readonly SRC_MAIN="main.c"
readonly SRC_PRIORITY=("priorityR.c" "priorityW.c")
readonly SRC_RAND="../../helpers/my_rand.c"
readonly SRC_GLOBALS="globals/globals.c"
readonly SRC_HELPERS="globals/helpers.c"
readonly EXEC="./executable"

readonly THREADS_START=2
readonly THREADS_INCREMENT=2
readonly THREADS_STEPS=4         # 2, 4, 6, 8
readonly RUNS_PER_THREAD=3

declare -A PROGRAM_TIMES    # Global array store thread averages per program

# ==============================================================================
# Compile Function
# ==============================================================================
compile_program() {
    # Local variable for function argument (lowercase)
    local priority_src="$1"
    
    # Using global constants
    gcc -o "$EXEC" "$SRC_MAIN" "$priority_src" "$SRC_RAND" "$SRC_GLOBALS" "$SRC_HELPERS" -lpthread
}

# ==============================================================================
# Benchmark Function
# Arguments: $1 = priority source file, $2..$5 = program parameters
# ==============================================================================
run_benchmark() {
    # Local variables for function arguments (lowercase)
    local src="$1"
    local a="$2"
    local b="$3"
    local c="$4"
    local pth="$5"

    local name
    name=$(basename "$src" .c)

    echo "======================================================"
    echo " -->  Compiling $src"
    echo "======================================================"

    if ! compile_program "$src"; then
        echo "Compilation failed for $src — skipping."
        return 1
    fi

    local num_threads=$THREADS_START # THREADS_START is constant
    local total_sum=0
    local thread_times=()

    # Loop variables (lowercase)
    for ((i=1; i<=THREADS_STEPS; i++)); do
        local sum_run=0

        echo "  Running with $num_threads threads..."

        # Loop variables (lowercase)
        for ((j=1; j<=RUNS_PER_THREAD; j++)); do
            # Temporary variables (lowercase)
            output=$("$EXEC" "$a" "$b" "$c" "$pth" "$num_threads")

            # Extract your expected output format:
            # "Elapsed time: X.XXXXXX"
            time_val=$(echo "$output" | awk '/Elapsed time/ {printf "%.6f", $4}')

            sum_run=$(awk "BEGIN {print $sum_run + $time_val}")
            echo -n "."
        done

        echo ""

        # Temporary variables (lowercase)
        local avg=$(awk "BEGIN {print $sum_run / $RUNS_PER_THREAD}")
        thread_times+=("$avg")

        echo "  Avg time for $num_threads threads: $avg s"

        total_sum=$(awk "BEGIN {print $total_sum + $sum_run}") #use awk as a calculator here --> The BEGIN { ... } block runs before any input is read

        # THREADS_INCREMENT is constant
        num_threads=$((num_threads + THREADS_INCREMENT)) 
    done

    # Compute overall average (lowercase)
    local total_runs=$((THREADS_STEPS * RUNS_PER_THREAD))
    local overall_avg
    overall_avg=$(awk "BEGIN {print $total_sum / $total_runs}")

    echo "--- Results for $name ---"
    echo "Thread Averages: ${thread_times[*]}"
    echo "Overall Average: $overall_avg s"

    # Store results in the global associative array (PROGRAM_TIMES)
    PROGRAM_TIMES["$name"]="${thread_times[*]}"
}

# ==============================================================================
# MAIN EXECUTION
# ==============================================================================

if [ $# -ne 4 ]; then
    echo "Usage: $0 <rowsA> <columnsA_rowsB> <columnsB> <numOfPth>"
    exit 1
fi

echo "Running priorityR..."
run_benchmark "${SRC_PRIORITY[0]}" "$1" "$2" "$3" "$4"

echo ""
echo "Running priorityW..."
run_benchmark "${SRC_PRIORITY[1]}" "$1" "$2" "$3" "$4"

[[ -f $EXEC ]] && rm "$EXEC"

echo "======================================================"
echo " --> ALL BENCHMARKS COMPLETE"
echo "Thread Averages per Program:"
for prog in "${!PROGRAM_TIMES[@]}"; do
    echo "  $prog: ${PROGRAM_TIMES[$prog]}"
done
echo "======================================================"
