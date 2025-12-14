# Makefile for DVA340 Lab1 - with executables in lab1 root
CC = gcc
CFLAGS = -Wall -std=c11

all: knapsack spain

# Knapsack (from your previous lab)
knapsack: src/knapsack.c
	$(CC) $(CFLAGS) src/knapsack.c -o knapsack

# Spain search (new)
spain: src/spain_search.c
	$(CC) $(CFLAGS) src/spain_search.c -o spain_search

# Run targets
run-knapsack: knapsack
	./knapsack

run-spain: spain
	./spain_search

# Clean everything
clean:
	rm -f knapsack spain_search

# Check data files exist
check-data:
	@echo "Checking data files..."
	@if [ -f "data/knapsack.txt" ]; then \
		echo "✓ data/knapsack.txt found"; \
	else \
		echo "✗ data/knapsack.txt missing"; \
	fi
	@if [ -f "data/spain.txt" ]; then \
		echo "✓ data/spain.txt found"; \
	else \
		echo "✗ data/spain.txt missing"; \
	fi

.PHONY: all clean run-knapsack run-spain check-data