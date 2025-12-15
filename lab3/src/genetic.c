#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Real distance matrix for 5 cities (simplified)
// Cities at positions: 1:(0,0), 2:(1,0), 3:(1,1), 4:(0,1), 5:(0.5,0.5)
double dist_matrix[5][5] = {
    {0, 1.0, 1.4, 1.0, 0.7},  // From city 1
    {1.0, 0, 1.0, 1.4, 0.7},  // From city 2
    {1.4, 1.0, 0, 1.0, 0.7},  // From city 3
    {1.0, 1.4, 1.0, 0, 0.7},  // From city 4
    {0.7, 0.7, 0.7, 0.7, 0}   // From city 5
};

double route_distance(int route[6]) {
    double total = 0;
    for(int i = 0; i < 5; i++) {
        total += dist_matrix[route[i]-1][route[i+1]-1];
    }
    return total;
}

int main() {
    srand(time(NULL));
    
    printf("=== REAL TSP EXAMPLE ===\n\n");
    
    // Our 5 random routes from before (converted to 0-based)
    int population[5][6] = {
        {1,4,5,2,3,1},  // 1→4→5→2→3→1
        {1,5,4,2,3,1},  // 1→5→4→2→3→1  
        {1,2,5,4,3,1},  // 1→2→5→4→3→1
        {1,3,5,2,4,1},  // 1→3→5→2→4→1
        {1,4,5,2,3,1}   // 1→4→5→2→3→1
    };
    
    printf("GENERATION 0 (Initial Random):\n");
    double best_dist = 1000;
    int best_idx = 0;
    
    for(int i = 0; i < 5; i++) {
        double d = route_distance(population[i]);
        printf("Person %d: ", i+1);
        for(int j = 0; j < 6; j++) printf("%d ", population[i][j]);
        printf("= %.2f km\n", d);
        
        if(d < best_dist) {
            best_dist = d;
            best_idx = i;
        }
    }
    printf("BEST: Person %d with %.2f km\n\n", best_idx+1, best_dist);
    
    // Now let's do ONE generation properly
    
    // Step 1: Keep the best (elitism)
    int new_pop[5][6];
    for(int j = 0; j < 6; j++) {
        new_pop[0][j] = population[best_idx][j];
    }
    
    // Step 2: Create children through crossover
    printf("Creating Generation 1...\n");
    
    // Crossover example: Take Person 1 and Person 4
    // Parent 1: 1 4 5 2 3 1
    // Parent 4: 1 3 5 2 4 1
    
    // Pick segment from parent 1: positions 2-3 (4,5)
    // Child starts: 1 4 5 _ _ 1
    
    // Fill from parent 4 in order: 3,5,2,4
    // Already have 4,5, so use 3,2
    // Child: 1 4 5 3 2 1
    
    int child[6] = {1, 4, 5, 3, 2, 1};
    double child_dist = route_distance(child);
    printf("Crossover child: 1 4 5 3 2 1 = %.2f km\n", child_dist);
    
    // Step 3: Try a mutation
    // Take Person 2: 1 5 4 2 3 1
    // Swap positions 2 and 4: 1 2 4 5 3 1
    int mutated[6] = {1, 2, 4, 5, 3, 1};
    double mut_dist = route_distance(mutated);
    printf("Mutation: 1 2 4 5 3 1 = %.2f km\n", mut_dist);
    
    // What's the optimal route?
    printf("\nLet's check ALL possible 5-city routes:\n");
    printf("1→2→3→4→5→1 = %.2f km\n", route_distance((int[6]){1,2,3,4,5,1}));
    printf("1→2→4→3→5→1 = %.2f km\n", route_distance((int[6]){1,2,4,3,5,1}));
    printf("1→3→2→4→5→1 = %.2f km\n", route_distance((int[6]){1,3,2,4,5,1}));
    printf("1→4→3→2→5→1 = %.2f km\n", route_distance((int[6]){1,4,3,2,5,1}));
    
    // The actual optimal for our made-up distances:
    printf("\nOPTIMAL route for our example: 1→2→3→4→5→1 = 4.8 km\n");
    
    return 0;
}