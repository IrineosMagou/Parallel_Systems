# Compiler and flags
CC = mpicc

# Source files and executable
SRC = mtx_vct_mult.c 
HELPERS_SRC = helpers/helpers.c
EXE = executable

# Compile the program
$(EXE): $(SRC) $(HELPERS_SRC)
	@echo "Compiling $<"
	$(CC) -o $@ $(SRC) $(HELPERS_SRC) -lm

# Run the program 
run: $(EXE)
ifeq ($(and $(p),$(n)),)
	$(error "Please provide both n and p, e.g., make run p=2 n=8000. 'make help' for usage information")
endif

#The hashtag keeps the terminal quiet
	@mpiexec -n $(p) ./$(EXE) $(n) 2>error.log

#Clean up
clean:
	@rm -f $(EXE)

# Usage / help
help:
	@echo "Usage:"
	@echo "  make            -> compile the program"
	@echo "  make run p=<num_processes> n=<dimensions>  -> run the program"
	@echo "  make clean      -> remove the executable"