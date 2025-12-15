#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 9
#define MAX_SUDOKUS 10

//Check if number can be placed
bool is_valid(int grid[SIZE][SIZE], int row, int col, int num) {
    //Check row
    for (int x = 0; x < SIZE; x++) {
        if (grid[row][x] == num) {
            return false;
        }
    }
    
    //Check column
    for (int x = 0; x < SIZE; x++) {
        if (grid[x][col] == num) {
            return false;
        }
    }
    
    //Check 3x3 box
    int box_row = row - row % 3;
    int box_col = col - col % 3;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[box_row + i][box_col + j] == num) {
                return false;
            }
        }
    }
    
    return true;
}

//Find next empty cell
bool find_empty(int grid[SIZE][SIZE], int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (grid[*row][*col] == 0) {
                return true;
            }
        }
    }
    return false;
}

//Backtracking solver
bool solve(int grid[SIZE][SIZE]) {
    int row, col;
    
    //If no empty cells, puzzle is solved
    if (!find_empty(grid, &row, &col)) {
        return true;
    }
    
    //Try numbers 1 through 9
    for (int num = 1; num <= 9; num++) {
        //Check if valid
        if (is_valid(grid, row, col, num)) {
            //Try placing number
            grid[row][col] = num;
            
            //Recursively try to solve 
            if (solve(grid)) {
                return true;
            }
            
            // If we get here, our choice didn't work
            // Backtrack (undo the placement)
            grid[row][col] = 0;
        }
    }
    
    //No number worked, need to backtrack further
    return false;
}

//Print the Sudoku grid
void print_grid(int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        if (row % 3 == 0 && row != 0) {
            printf("------+-------+------\n");
        }
        
        for (int col = 0; col < SIZE; col++) {
            if (col % 3 == 0 && col != 0) {
                printf("| ");
            }
            
            if (grid[row][col] == 0) {
                printf(". ");
            } else {
                printf("%d ", grid[row][col]);
            }
        }
        printf("\n");
    }
}

//Read all Sudokus from file
int read_sudokus_from_file(char* filename, int sudokus[][SIZE][SIZE]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return 0;
    }
    
    char line[50];
    int current_sudoku = -1;
    int row = 0;
    int count = 0;
    
    //Read until EOF
    while (fgets(line, sizeof(line), file) != NULL) {
        //Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        //Skip empty lines
        if (strlen(line) == 0) continue;
        
        //Stop at EOF marker
        if (strstr(line, "EOF") != NULL) break;
        
        //Skip other header lines
        if (strstr(line, "NAME:") != NULL) continue;
        if (strstr(line, "TYPE:") != NULL) continue;
        if (strstr(line, "COMMENT:") != NULL) continue;
        
        //Check if this is a new Sudoku header
        if (strstr(line, "SUDOKU") != NULL) {
            current_sudoku++;
            row = 0;
            continue;
        }
        
        //Only process if we have a valid Sudoku and valid line
        if (current_sudoku >= 0 && current_sudoku < MAX_SUDOKUS && 
            row < SIZE && strlen(line) >= SIZE) {
            for (int col = 0; col < SIZE; col++) {
                sudokus[current_sudoku][row][col] = line[col] - '0';
            }
            row++;
            
            //If completed a Sudoku
            if (row == SIZE) {
                count = current_sudoku + 1; 
            }
        }
    }
    
    fclose(file);
    return count;
}

int main() {
    printf("SUDOKU SOLVER (Backtracking/DFS)\n");
    
    //Array to store all puzzles
    int sudokus[MAX_SUDOKUS][SIZE][SIZE];
    int sudoku_count = 0;
    
    //Read from file
    sudoku_count = read_sudokus_from_file("data/sudoku.txt", sudokus);
    
    
    printf("Found %d Sudoku puzzle(s)\n\n", sudoku_count);
    
    //Solve each puzzle
    for (int s = 0; s < sudoku_count; s++) {
        printf("SUDOKU #%d\n", s + 1);
        
        printf("Initial puzzle:\n");
        print_grid(sudokus[s]);
        printf("\n");
        
        //Make a copy to solve 
        int grid[SIZE][SIZE];
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j] = sudokus[s][i][j];
            }
        }
        
        printf("Solving...\n");
        
        if (solve(grid)) {
            printf("\nSolved puzzle:\n");
            print_grid(grid);
        } else {
            printf("\nNo solution found!\n");
        }
        
        printf("\n");
    }
    
    
    printf("All %d Sudoku(s) processed.\n", sudoku_count);
    
    return 0;
}