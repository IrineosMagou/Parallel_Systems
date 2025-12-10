# Makefile
# This Makefile takes input one of the two thread tasks source files 
# of this directory and uses it to build and compile the program

# The compiler and flags
CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lpthread # Example linker flags (like math library)
SRC = main.c
HELPERS_SRC = ../../helpers/my_rand.c
GLOBALS_SRC = globals/globals.c globals/helpers.c
EXE = executable
# A generic target to compile any executable
# Target name: 'build'
# This target takes the 'SRC' variable from the command line
build: $(SHARED_OBJ)
	@if [ -z "$(P_SRC)" ]; then \
		echo "Error: Must provide the source file (e.g., make build SRC=file1.c)"; \
		exit 1; \
	fi
	@echo "--- Compiling $(SRC) with $(P_SRC) ---"
	$(CC) $(CFLAGS) -o $(EXE) $(SRC) $(P_SRC) $(GLOBALS_SRC) $(HELPERS_SRC) $(LDFLAGS)

run: $(EXE)
ifeq ($(and $(m), $(n), $(p), $(k), $(t)),)
	$(error "Please provide keys, total operations, searches and inserts \
	 make run m=1000 n=500000 p=0.95 k=0.03 t=2. 'make help' for usage information")
endif
	@./$(EXE) $(m) $(n) $(p) $(k) $(t) 

clean:
	@rm -f $(EXE)

# Usage / help
help:
	@echo "Usage:"
	@echo "  make build P_SRC=filename.c  -> compile the program"
	@echo "  make run m=<m> n=<n> p=<p> k=<k> t=<t>  -> run the program"
	@echo "  make clean      -> remove the executable"