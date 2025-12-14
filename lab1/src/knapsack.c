#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 20

// ========== File Reading ==========
int read_data(const char* filename, int values[], int weights[], int* capacity) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        printf("Make sure the file exists in the data/ folder\n");
        return 0;
    }
    
    char line[100];
    int count = 0;
    int reading = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        // Skip empty lines
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

// ========== DFS Algorithm ==========
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
    
    // Take item
    if (weight + dfs_weights[level] <= dfs_capacity) {
        dfs(level + 1, value + dfs_values[level], weight + dfs_weights[level]);
    }
    
    // Skip item
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
    
    if (time_taken > 0.5) {
        printf("Warning: DFS took more than 0.5 seconds!\n");
    }
    
    return dfs_best;
}

// ========== BFS Algorithm ==========
#define QUEUE_SIZE 50000

typedef struct {
    int level, value, weight;
} Node;

int bfs(const char* filename) {
    int values[MAX_ITEMS], weights[MAX_ITEMS];
    int capacity, n;
    
    n = read_data(filename, values, weights, &capacity);
    if (n == 0) return -1;
    
    Node queue[QUEUE_SIZE];
    int front = 0, rear = 0;
    int best = 0;
    
    // Start with empty knapsack
    queue[rear].level = 0;
    queue[rear].value = 0;
    queue[rear].weight = 0;
    rear++;
    
    clock_t start = clock();
    
    while (front < rear) {
        Node current = queue[front++];
        
        // All items processed?
        if (current.level == n) {
            if (current.value > best && current.weight <= capacity) {
                best = current.value;
            }
            continue;
        }
        
        // Take item (if it fits)
        if (current.weight + weights[current.level] <= capacity) {
            queue[rear].level = current.level + 1;
            queue[rear].value = current.value + values[current.level];
            queue[rear].weight = current.weight + weights[current.level];
            rear++;
            
            if (queue[rear-1].value > best) {
                best = queue[rear-1].value;
            }
        }
        
        // Skip item
        queue[rear].level = current.level + 1;
        queue[rear].value = current.value;
        queue[rear].weight = current.weight;
        rear++;
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("BFS Time: %.6f seconds\n", time_taken);
    
    if (time_taken > 0.5) {
        printf("Warning: BFS took more than 0.5 seconds!\n");
    }
    
    return best;
}

// ========== Print items ==========
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

// ========== Main ==========
int main() {
    const char* filename = "../data/knapsack.txt";  // Path from src/ to data/
    
    printf("================================\n");
    printf("  Knapsack 0/1 - BFS vs DFS\n");
    printf("================================\n\n");
    
    // Print loaded items
    print_items(filename);
    
    printf("Running algorithms...\n");
    printf("---------------------\n");
    
    // Run DFS
    int dfs_result = solve_dfs(filename);
    printf("DFS Result: %d\n\n", dfs_result);
    
    // Run BFS  
    int bfs_result = bfs(filename);
    printf("BFS Result: %d\n\n", bfs_result);
    
    // Compare results
    if (dfs_result == bfs_result) {
        printf("✓ Both algorithms found the same optimal value: %d\n", dfs_result);
    } else {
        printf("✗ ERROR: Results differ! DFS=%d, BFS=%d\n", dfs_result, bfs_result);
    }
    
    printf("\n================================\n");
    return 0;
}