CC = gcc
CFLAGS = -Wall -std=c11

all: lab1 lab2

# Lab1: Knapsack & Spain
lab1: lab1/build lab1/src/knapsack.c lab1/src/spain_search.c
	$(CC) $(CFLAGS) lab1/src/knapsack.c -o lab1/build/knapsack
	$(CC) $(CFLAGS) lab1/src/spain_search.c -o lab1/build/spain_search

run-knap: lab1
	./lab1/build/knapsack lab1/data/knapsack.txt

run-spain: lab1
	./lab1/build/spain_search lab1/data/spain.txt

# Lab2: Sudoku
lab2: lab2/build lab2/src/sudoku.c
	$(CC) $(CFLAGS) lab2/src/sudoku.c -o lab2/build/sudoku

run-sudoku: lab2
	./lab2/build/sudoku lab2/data/sudoku.txt

.PHONY: all lab1 lab2 run-knap run-spain run-sudoku clean

