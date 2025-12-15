CC = gcc
CFLAGS = -Wall -std=c11

all: sudoku

# Compile Sudoku
sudoku: src/sudoku.c
	$(CC) $(CFLAGS) src/sudoku.c -o sudoku

# Run target
run: sudoku
	./sudoku data/sudoku.txt

# Clean everything
clean:
	rm -f sudoku

# Check data file exists
check-data:
	@echo "Checking data file..."
	@if [ -f "data/sudoku.txt" ]; then \
		echo "✓ data/sudoku.txt found"; \
	else \
		echo "✗ data/sudoku.txt missing"; \
	fi

.PHONY: all clean run check-data