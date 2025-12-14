#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CITIES 100
#define MAX_NAME 50

typedef struct {
    char from[MAX_NAME], to[MAX_NAME];
    int dist;
} Road;

typedef struct {
    char name[MAX_NAME];
    int h;  // heuristic to Valladolid
} City;

Road roads[200];
City cities[MAX_CITIES];
int road_count = 0, city_count = 0;

// Simple data loading
void load_data() {
    FILE* f = fopen("data/spain.txt", "r");
    if (!f) {
        printf("Error: Cannot open data/spain.txt\n");
        exit(1);
    }
    
    char line[200];
    int read_roads = 0;
    
    while (fgets(line, sizeof(line), f)) {
        // Skip empty lines and headers
        if (strlen(line) < 3) continue;
        if (strstr(line, "NAME:")) continue;
        if (strstr(line, "TYPE:")) continue;
        if (strstr(line, "COMMENT:")) continue;
        
        // Start reading roads
        if (strstr(line, "A B Distance")) {
            read_roads = 1;
            continue;
        }
        
        // Start reading heuristics
        if (strstr(line, "Straight line")) {
            read_roads = 0;
            continue;
        }
        
        char a[MAX_NAME], b[MAX_NAME];
        int d;
        
        if (read_roads) {
            // Read road: CityA CityB Distance
            if (sscanf(line, "%s %s %d", a, b, &d) == 3) {
                strcpy(roads[road_count].from, a);
                strcpy(roads[road_count].to, b);
                roads[road_count].dist = d;
                road_count++;
            }
        } else {
            // Read heuristic: City Distance
            if (sscanf(line, "%s %d", a, &d) == 2) {
                strcpy(cities[city_count].name, a);
                cities[city_count].h = d;
                city_count++;
            }
        }
    }
    fclose(f);
    
    printf("Loaded %d roads, %d cities\n", road_count, city_count);
}

int get_heuristic(char* city) {
    for (int i = 0; i < city_count; i++) {
        if (strcmp(cities[i].name, city) == 0)
            return cities[i].h;
    }
    return 9999;
}

int get_distance(char* a, char* b) {
    for (int i = 0; i < road_count; i++) {
        if ((strcmp(roads[i].from, a) == 0 && strcmp(roads[i].to, b) == 0) ||
            (strcmp(roads[i].from, b) == 0 && strcmp(roads[i].to, a) == 0)) {
            return roads[i].dist;
        }
    }
    return -1;
}

// Node for search
typedef struct {
    char city[MAX_NAME];
    int cost;  // g(n) for A*, h(n) for Greedy
    int total; // f(n) for A* (g+h), same as cost for Greedy
    int parent;
} Node;

Node nodes[1000];
int node_count = 0;

int make_node(char* city, int cost, int total, int parent) {
    strcpy(nodes[node_count].city, city);
    nodes[node_count].cost = cost;
    nodes[node_count].total = total;
    nodes[node_count].parent = parent;
    return node_count++;
}

void print_path(int idx) {
    if (idx == -1) return;
    print_path(nodes[idx].parent);
    if (nodes[idx].parent != -1) printf(" -> ");
    printf("%s", nodes[idx].city);
}

// GREEDY: uses h(n) only
void greedy_search() {
    printf("\n=== GREEDY BEST-FIRST ===\n");
    
    Node* open[1000];
    int open_count = 0;
    char visited[100][MAX_NAME];
    int visited_count = 0;
    
    // Start node
    int start_h = get_heuristic("Malaga");
    int start_idx = make_node("Malaga", start_h, start_h, -1);
    open[open_count++] = &nodes[start_idx];
    strcpy(visited[visited_count++], "Malaga");
    
    while (open_count > 0) {
        // Find node with smallest h (most promising)
        int best = 0;
        for (int i = 1; i < open_count; i++) {
            if (open[i]->cost < open[best]->cost)  // cost = h(n) for greedy
                best = i;
        }
        
        Node* current = open[best];
        
        // Remove from open list
        for (int i = best; i < open_count - 1; i++)
            open[i] = open[i + 1];
        open_count--;
        
        printf("Expanding: %s (h=%d)\n", current->city, current->cost);
        
        // Goal check
        if (strcmp(current->city, "Valladolid") == 0) {
            printf("\n✓ Greedy path found!\nPath: ");
            print_path(current - nodes);  // Get index
            printf("\nTotal distance: %d km\n", current->total);
            return;
        }
        
        // Expand neighbors
        for (int i = 0; i < road_count; i++) {
            char* neighbor = NULL;
            if (strcmp(roads[i].from, current->city) == 0)
                neighbor = roads[i].to;
            else if (strcmp(roads[i].to, current->city) == 0)
                neighbor = roads[i].from;
            
            if (neighbor) {
                // Check if visited
                int seen = 0;
                for (int j = 0; j < visited_count; j++) {
                    if (strcmp(visited[j], neighbor) == 0) {
                        seen = 1;
                        break;
                    }
                }
                
                if (!seen) {
                    int h = get_heuristic(neighbor);
                    int idx = make_node(neighbor, h, h, current - nodes);
                    open[open_count++] = &nodes[idx];
                    strcpy(visited[visited_count++], neighbor);
                    printf("  -> %s (h=%d)\n", neighbor, h);
                }
            }
        }
    }
    printf("\n✗ No path found!\n");
}

// A*: uses g(n) + h(n)
void astar_search() {
    printf("\n=== A* SEARCH ===\n");
    
    Node* open[1000];
    int open_count = 0;
    int g_values[100];  // Track best g for each city
    char city_names[100][MAX_NAME];
    int cities_seen = 0;
    
    // Start node
    int start_h = get_heuristic("Malaga");
    int start_idx = make_node("Malaga", 0, start_h, -1);  // cost = g, total = f
    open[open_count++] = &nodes[start_idx];
    
    strcpy(city_names[cities_seen], "Malaga");
    g_values[cities_seen] = 0;
    cities_seen++;
    
    while (open_count > 0) {
        // Find node with smallest f
        int best = 0;
        for (int i = 1; i < open_count; i++) {
            if (open[i]->total < open[best]->total)
                best = i;
        }
        
        Node* current = open[best];
        
        // Remove from open
        for (int i = best; i < open_count - 1; i++)
            open[i] = open[i + 1];
        open_count--;
        
        printf("Expanding: %s (g=%d, h=%d, f=%d)\n", 
               current->city, current->cost, 
               current->total - current->cost,  // h = f - g
               current->total);
        
        // Goal check
        if (strcmp(current->city, "Valladolid") == 0) {
            printf("\n✓ A* optimal path found!\nPath: ");
            print_path(current - nodes);
            printf("\nTotal distance: %d km\n", current->cost);  // g = total distance
            return;
        }
        
        // Expand neighbors
        for (int i = 0; i < road_count; i++) {
            char* neighbor = NULL;
            if (strcmp(roads[i].from, current->city) == 0)
                neighbor = roads[i].to;
            else if (strcmp(roads[i].to, current->city) == 0)
                neighbor = roads[i].from;
            
            if (neighbor) {
                int road_cost = get_distance(current->city, neighbor);
                int g_new = current->cost + road_cost;
                int h_new = get_heuristic(neighbor);
                int f_new = g_new + h_new;
                
                // Check if we have a better path to this city
                int found = -1;
                for (int j = 0; j < cities_seen; j++) {
                    if (strcmp(city_names[j], neighbor) == 0) {
                        found = j;
                        break;
                    }
                }
                
                if (found == -1 || g_new < g_values[found]) {
                    // New or better path
                    int idx = make_node(neighbor, g_new, f_new, current - nodes);
                    open[open_count++] = &nodes[idx];
                    
                    if (found == -1) {
                        strcpy(city_names[cities_seen], neighbor);
                        g_values[cities_seen] = g_new;
                        cities_seen++;
                    } else {
                        g_values[found] = g_new;
                    }
                    
                    printf("  -> %s (g=%d, h=%d, f=%d)\n", 
                           neighbor, g_new, h_new, f_new);
                }
            }
        }
    }
    printf("\n✗ No path found!\n");
}

int main() {
    printf("SPAIN SEARCH: Malaga to Valladolid\n");
    printf("===================================\n");
    
    load_data();
    greedy_search();
    astar_search();
    
    return 0;
}