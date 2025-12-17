# Compiler and flags
CC = gcc
CFLAGS = -lpthread

# Source files and executable
SRC = monte_carlo.c 
HELPERS_SRC = ../../helpers/my_rand.c
EXE = executable
# .PHONY: all run clean help

# # Default target
# all: $(EXE)

# Compile the program
$(EXE): $(SRC)
	$(CC) -o $@ $(SRC) $(HELPERS_SRC) $(CFLAGS)

# Run the program 
run: $(EXE)
ifeq ($(and $(n),$(t)),)
	$(error "Please provide both n and t, e.g., make run n=1000000 t=2. 'make help' for usage information")
endif
#The hashtag keeps the terminal quiet
	@./$(EXE) $(n) $(t)

# Clean up
clean:
	@rm -f $(EXE)

# Usage / help
help:
	@echo "Usage:"
	@echo "  make            -> compile the program"
	@echo "  make run n=<num_samples> t=<num_threads>  -> run the program"
	@echo "  make clean      -> remove the executable"
