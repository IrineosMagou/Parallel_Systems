# Makefile
# This Makefile takes input one of the three source files 
# of this directory and uses it to build and compile the program

# The compiler and flags
CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lpthread 
HELPERS_SRC = helpers/matrix_mul_helpers.c
EXE = executable

# This target takes the 'SRC' variable from the command line
build: $(SHARED_OBJ)
	@if [ -z "$(SRC)" ]; then \
		echo "Error: Must provide the source file (e.g., make build SRC=file1.c)"; \
		exit 1; \
	fi
	@echo "--- Compiling $(SRC) ---"
	$(CC) $(CFLAGS) -o $(EXE) $(SRC) $(HELPERS_SRC) $(LDFLAGS) 

run: $(EXE)
ifeq ($(and $(m), $(n), $(p), $(t)),)
	$(error "Please provide matrix dimensions m, n, p and number of threads e.g., \
	 make run m=8000 n=8000 p=80 t=2. 'make help' for usage information")
endif
	@./$(EXE) $(m) $(n) $(p) $(t)

# Clean up rule
clean:
	@rm -f $(EXE)
# Usage / help
help:
	@echo "Usage:"
	@echo "  make build SRC=filename.c  -> compile the program"
	@echo "  make run m=<m> n=<n> p=<p> t=<t>  -> run the program"
	@echo "  make clean      -> remove the executable"
