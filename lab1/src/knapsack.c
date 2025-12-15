#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 12

//File Reading
int read_data(const char* filename, int values[], int weights[], int* capacity) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file '%s'\n", filename);
        return 0;
    }
    
    char line[100];
    int count = 0;
    int reading = 0;
    
    while (fgets(line, sizeof(line), file)) {
        //Remove newline
        line[strcspn(line, "\n")] = 0;
        
        //Skip empty lines
        if (strlen(line) == 0) continue;
        
        if (strstr(line, "MAXIMUM WEIGHT:")) {
            sscanf(line, "MAXIMUM WEIGHT: %d", capacity);
            continue;
        }
        
        if (strstr(line, "ID b w")) {
            reading = 1;
            continue;
        }
        
        if (strstr(line, "EOF")) {
            break;
        }
        
        if (reading) {
            int id, v, w;
            if (sscanf(line, "%d %d %d", &id, &v, &w) == 3) {
                values[count] = v;
                weights[count] = w;
                count++;
            }
        }
    }
    
    fclose(file);
    
    if (count == 0) {
        printf("Warning: No items loaded from file\n");
    }
    
    return count;
}

//DFS Algorithm
int dfs_best = 0;
int dfs_values[MAX_ITEMS], dfs_weights[MAX_ITEMS];
int dfs_capacity, dfs_n;

void dfs(int level, int value, int weight) {
    if (level == dfs_n) {
        if (value > dfs_best && weight <= dfs_capacity) {
            dfs_best = value;
        }
        return;
    }
    
    //Take item
    if (weight + dfs_weights[level] <= dfs_capacity) {
        dfs(level + 1, value + dfs_values[level], weight + dfs_weights[level]);
    }
    
    //Skip item
    dfs(level + 1, value, weight);
}

int solve_dfs(const char* filename) {
    dfs_best = 0;
    dfs_n = read_data(filename, dfs_values, dfs_weights, &dfs_capacity);
    
    if (dfs_n == 0) return -1;
    
    clock_t start = clock();
    dfs(0, 0, 0);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("DFS Time: %.6f seconds\n", time_taken);
    
    return dfs_best;
}

//BFS Algorithm
typedef struct {
    int level, value, weight;
} Node;

int bfs(const char* filename) {
    int values[MAX_ITEMS], weights[MAX_ITEMS];
    int capacity, n;
    
    n = read_data(filename, values, weights, &capacity);
    if (n == 0) return -1;
    
    #define QUEUE_SIZE 2048  //Try smaller?
    
    Node queue[QUEUE_SIZE];
    int front = 0, rear = 0;
    int best = 0;
    int max_used = 0;
    
    queue[rear++] = (Node){0, 0, 0};

    clock_t start = clock();
    
    while (front < rear) {
        //Track maximum queue usage
        if (rear - front > max_used) {
            max_used = rear - front;
        }
       
        Node current = queue[front++];
        
        if (current.level == n) {
            if (current.value > best && current.weight <= capacity) {
                best = current.value;
            }
            continue;
        }
       //Skip item branch
    if (current.level + 1 == n) {  // Would create leaf
    //Evaluate leaf immediately
    if (current.value > best && current.weight <= capacity) {
        best = current.value;
    }
    } else {  //Not a leaf 
        queue[rear++] = (Node){current.level + 1, current.value, current.weight};
    }

    //Take item branch (if weight allows)
    if (current.weight + weights[current.level] <= capacity) {
        int new_level = current.level + 1;
        int new_value = current.value + values[current.level];
        int new_weight = current.weight + weights[current.level];
    
    if (new_level == n) {  //Would create leaf
        //Evaluate leaf immediately
        if (new_value > best) {  // Weight already checked above
            best = new_value;
        }
    } else {  //Not a leaf
        queue[rear++] = (Node){new_level, new_value, new_weight};
    }
    }
}
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("BFS Time: %.6f seconds\n", time_taken);
    
    return best;
}


//Print items
void print_items(const char* filename) {
    int values[MAX_ITEMS], weights[MAX_ITEMS];
    int capacity, n;
    
    n = read_data(filename, values, weights, &capacity);
    
    if (n == 0) return;
    
    printf("\nLoaded %d items, Capacity: %d\n", n, capacity);
    printf("ID\tValue\tWeight\tRatio\n");
    printf("--\t-----\t------\t-----\n");
    
    for (int i = 0; i < n; i++) {
        double ratio = (double)values[i] / weights[i];
        printf("%d\t%d\t%d\t%.2f\n", i+1, values[i], weights[i], ratio);
    }
    printf("\n");
}


int main() {
    const char* filename = "data/knapsack.txt";  
    
    printf("================================\n");
    printf("  Knapsack 0/1 - BFS vs DFS\n");
    printf("================================\n\n");
    
    //Print loaded items
    print_items(filename);
    
    printf("Running algorithms...\n");
    printf("---------------------\n");
    
    //Run DFS
    int dfs_result = solve_dfs(filename);
    printf("DFS Result: %d\n\n", dfs_result);
    
    //Run BFS  
    int bfs_result = bfs(filename);
    printf("BFS Result: %d\n\n", bfs_result);
    
    //Compare results
    if (dfs_result == bfs_result) {
        printf("Both algorithms found the same optimal value: %d\n", dfs_result);
    } else {
        printf("ERROR: Results differ! DFS=%d, BFS=%d\n", dfs_result, bfs_result);
    }
    
    printf("\n================================\n");
    return 0;
}