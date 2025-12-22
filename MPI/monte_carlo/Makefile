# Compiler and flags
CC = mpicc

# Source files and executable
SRC = monte_carlo.c 
HELPERS_SRC = ../../helpers/my_rand.c
EXE = executable

# Compile the program
$(EXE): $(SRC) $(HELPERS_SRC)
	@echo "Compiling $<"
	$(CC) -o $@ $(SRC) $(HELPERS_SRC)

# Run the program 
run: $(EXE)
ifeq ($(and $(p),$(n)),)
	$(error "Please provide both n and p, e.g., make run p=2 n=1000000. 'make help' for usage information")
endif

#The hashtag keeps the terminal quiet
	@mpiexec -n $(p) ./$(EXE) $(n)

#Clean up
clean:
	@rm -f $(EXE)

# Usage / help
help:
	@echo "Usage:"
	@echo "  make            -> compile the program"
	@echo "  make run n=<num_samples> t=<num_processes>  -> run the program"
	@echo "  make clean      -> remove the executable"