# Makefile
# This Makefile takes input one of the two source files 
# of this directory and uses it to build and compile the program

# The compiler and flags
CC = gcc
CFLAGS = -Wall 
LDFLAGS = -fopenmp 
HELPERS_SRC = helpers/helpers.c
EXE = executable
# A generic target to compile any executable
# Target name: 'build'
# This target takes the 'SRC' variable from the command line
build: $(SHARED_OBJ)
	@if [ -z "$(SRC)" ]; then \
		echo "Error: Must provide the source file (e.g., make build SRC=file1.c)"; \
		exit 1; \
	fi
	@echo "--- Compiling $(SRC) ---"
	$(CC) $(CFLAGS) -o $(EXE) $(SRC) $(HELPERS_SRC) $(LDFLAGS) 

run: $(EXE)
ifeq ($(and $(t)), $(m), $(n),)
	$(error "Please provide matrix dimensions m, n and number of threads e.g., \
	 make run t=2 m=8000 n=8000 . 'make help' for usage information")
endif
	@./$(EXE) $(t) $(m) $(n) 

# Clean up rule
clean:
	@rm -f $(EXE)
# Usage / help
help:
	@echo "Usage:"
	@echo "  make build SRC=filename.c  -> compile the program"
	@echo "  make run t=<t> m=<m> n=<n>  -> run the program"
	@echo "  make clean      -> remove the executable"
