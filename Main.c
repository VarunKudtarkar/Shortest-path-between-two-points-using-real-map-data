#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAX_NODES 1000  // Increased to handle larger graphs
#define MAX_K_PATHS 10  // Maximum number of alternative paths to find
#define AMBULANCE_PRIORITY 1
#define POLICE_PRIORITY 2
#define BUS_PRIORITY 3
#define CAR_PRIORITY 4
#define TWO_WHEELER_PRIORITY 5
#define WALKING_PRIORITY 6
#define MAX_VEHICLE_TYPE 20

// Keep existing structs from the original code
typedef struct Node {
    char* name;
    double distance;
    struct Node* next;
} Node;

typedef struct Graph {
    Node** nodes;
    int size;
} Graph;

// New struct to represent a path for Yen's algorithm
typedef struct Path {
    char** nodes;
    int length;
    double total_distance;
} Path;

// Yen's Algorithm support structures
typedef struct {
    Path* paths;
    int num_paths;
    int capacity;
} KShortestPaths;

typedef struct {
    char* intersection;
    double distance;
} QueueNode;

typedef struct {
    QueueNode* nodes;
    int size;
    int capacity;
} PriorityQueue;

typedef struct {
    char* identifier;  // The numeric node identifier from the input file
    char* name;        // The human-readable name for display
} NodeMapping;

typedef struct {
    char name[50];
    char address[200];
} Location;


// Existing priority queue and other structs remain the same
// ... (keep all previous struct definitions)

// Function prototypes for Yen's algorithm
Path* create_path();
void free_path(Path* path);
KShortestPaths* create_k_shortest_paths(int k);
void free_k_shortest_paths(KShortestPaths* k_paths);
int path_contains_node(Path* path, const char* node);
Path* clone_path(Path* original);
void remove_link(Graph* graph, const char* from, const char* to);
void restore_link(Graph* graph, const char* from, const char* to, double weight);

// Existing function declarations
// ... (keep existing function declarations)

// Modify Dijkstra to return a Path instead of just displaying

PriorityQueue* create_priority_queue(int capacity) {
    PriorityQueue* pq = (PriorityQueue*) malloc(sizeof(PriorityQueue));
    if (!pq) exit(EXIT_FAILURE);  // Ensure malloc succeeded
    pq->nodes = (QueueNode*) malloc(capacity * sizeof(QueueNode));
    if (!pq->nodes) exit(EXIT_FAILURE);  // Ensure malloc succeeded
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void swap(QueueNode* a, QueueNode* b) {
    QueueNode temp = *a;
    *a = *b;
    *b = temp;
}

void insert(PriorityQueue* pq, QueueNode node) {
    if (pq->size == pq->capacity) {
        printf("Priority queue is full!\n");
        return;
    }

    pq->nodes[pq->size] = node;
    int i = pq->size;
    pq->size++;

    // Bubble up
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (pq->nodes[i].distance >= pq->nodes[parent].distance) break;
        swap(&pq->nodes[i], &pq->nodes[parent]);
        i = parent;
    }
}

QueueNode extract_min(PriorityQueue* pq) {
    if (pq->size == 0) {
        printf("Priority queue is empty!\n");
        return (QueueNode){NULL, INT_MAX};
    }

    QueueNode min = pq->nodes[0];
    pq->nodes[0] = pq->nodes[--pq->size];

    // Bubble down
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < pq->size && pq->nodes[left].distance < pq->nodes[smallest].distance) {
            smallest = left;
        }
        if (right < pq->size && pq->nodes[right].distance < pq->nodes[smallest].distance) {
            smallest = right;
        }
        if (smallest == i) 
            break;

        swap(&pq->nodes[i], &pq->nodes[smallest]);
        i = smallest;
    }
    return min;
}

int is_empty(PriorityQueue* pq) {
    return pq->size == 0;
}

void add_node(Graph* graph, const char* name) {
    Node* node = (Node*) malloc(sizeof(Node));
    if (!node) exit(EXIT_FAILURE);  // Ensure malloc succeeded
    node->name = strdup(name);
    node->distance = 0.0; // Initialize distance to 0
    node->next = NULL;

    graph->nodes[graph->size] = node;
    graph->size++;
}

int get_index(Graph* graph, const char* name) {
    for (int i = 0; i < graph->size; i++) {
        if (strcmp(graph->nodes[i]->name, name) == 0) return i;
    }
    return -1; // Not found
}


void add_edge(Graph* graph, const char* from, const char* to, double weight) {
    Node* from_node = graph->nodes[get_index(graph, from)];
    if (from_node == NULL){
        printf("Node %s not found in the graph. \n", from);
        return;
    }
    
    for(Node* temp = from_node->next; temp; temp = temp->next){
        if (strcmp(temp->name, to) == 0)
            return;     //edge already exists
    }

    Node* to_node = (Node*) malloc(sizeof(Node));
    if (!to_node) exit(EXIT_FAILURE);
    to_node->name = strdup(to);
    to_node->distance = weight;
    to_node->next = from_node->next;
    from_node->next = to_node;
}

/*void display_graph(Graph* graph) {
    for (int i = 0; i < graph->size; i++) {
        printf("%s ", graph->nodes[i]->name);
        for (Node* neighbor = graph->nodes[i]->next; neighbor != NULL; neighbor = neighbor->next) {
            printf("--%.3f--> %s ", neighbor->distance, neighbor->name);
        }
        printf("\n");
    }
}*/

const char* get_node_name(NodeMapping* mappings, int num_mappings, const char* identifier) {
    for (int i = 0; i < num_mappings; i++) {
        if (strcmp(mappings[i].identifier, identifier) == 0) {
            return mappings[i].name;
        }
    }
    return identifier; // Return identifier if no mapping is found
}

double getAverageSpeed(const char *vehicleType) {
    if (strcmp(vehicleType, "ambulance") == 0) {
        return 60.0;
    } else if (strcmp(vehicleType, "police") == 0) {
        return 65.0;
    } else if (strcmp(vehicleType, "bus") == 0) {
        return 10.0;
    } else if (strcmp(vehicleType, "car") == 0) {
        return 15.0;
    } else if (strcmp(vehicleType, "two_wheeler") == 0) {
        return 45.0;
    } else if (strcmp(vehicleType, "walking") == 0) {
        return 5.0;
    } else {
        printf("Invalid vehicle type! Using default speed of 50 km/h.\n");
        return 50.0;
    }
}

// Function to calculate travel time based on distance and vehicle type
double calculateTravelTime(double distance, const char *vehicleType) {
    double distanceInKm = distance / 1000.0;
    double avgSpeed = getAverageSpeed(vehicleType);
    //distanceInKm = 10;    testing
    double timeInHours = distanceInKm / avgSpeed;
    
    return timeInHours; 
}

void display_shortest_path(Graph* graph, NodeMapping* mappings, int num_mappings, const char* start, const char* end, char** previous, double* distances, char* vehicle_type) {
    int end_index = get_index(graph, end);

    if (end_index == -1 || distances[end_index] == INT_MAX) {
        printf("No path from %s to %s\n", start, end);
        return;
    }

    double final_distance = distances[end_index];
    final_distance /= 1000;
    printf("Shortest path from %s to %s: ", get_node_name(mappings, num_mappings, start), get_node_name(mappings, num_mappings, end));
    char* path[MAX_NODES];
    int path_index = 0;

    for (const char* current = end; current != NULL; current = previous[end_index]) {
        path[path_index++] = strdup(current);
        for (int i = 0; i < graph->size; i++) {
            if (strcmp(graph->nodes[i]->name, current) == 0) {
                end_index = i;
                break;
            }
        }
        if (strcmp(current, start) == 0) break;
    }

    for (int i = path_index - 1; i >= 0; i--) {
        printf("%s ", get_node_name(mappings, num_mappings, path[i]));
        free(path[i]);
    }   
    double time_taken = calculateTravelTime(1000 * final_distance, vehicle_type);
    int hours = (int)time_taken;
    int minutes = (int)((time_taken - hours) * 60);

    if (time_taken < 1.0) {
        minutes = (int)(time_taken * 60); // Convert the remaining time to minutes directly
        hours = 0;
    }

    if (minutes == 0 && time_taken > 0) {
        minutes = 1;  // If time is small but greater than 0, show at least 1 minute
    }
    
    printf("\nShortest Distance: %.3f km\n", final_distance);
    printf("Time Taken: %d hr %d min\n", hours, minutes);
}

double adjust_weight(double base_weight, int priority) {
    float traffic_multiplier;

    switch (priority) {
        case AMBULANCE_PRIORITY:
        case POLICE_PRIORITY:
            traffic_multiplier = 1.0;
            break;
        case TWO_WHEELER_PRIORITY:
            traffic_multiplier = 2.0;
            break;
        case BUS_PRIORITY:
        case CAR_PRIORITY:
            traffic_multiplier = 3.0;
            break;
        case WALKING_PRIORITY:
            traffic_multiplier = 1.0;
            break;
        default:
            traffic_multiplier = 1.0;
    }

    return base_weight * traffic_multiplier;
}

Path* dijkstra_single_path(Graph* graph, const char* start, const char* end, 
                            int vehicle_priority, NodeMapping* mappings, 
                            int num_mappings, char* vehicle_type) {
    double* distances = (double*) malloc(graph->size * sizeof(double));
    char** previous = (char**) malloc(graph->size * sizeof(char*));
    int* visited = (int*) calloc(graph->size, sizeof(int));

    for (int i = 0; i < graph->size; i++) {
        distances[i] = INT_MAX;
        previous[i] = NULL;
    }

    int start_index = get_index(graph, start);
    if (start_index == -1) {
        free(distances);
        free(previous);
        free(visited);
        return NULL;
    }

    distances[start_index] = 0.0;
    PriorityQueue* pq = create_priority_queue(graph->size);
    insert(pq, (QueueNode){strdup(start), 0.0});

    while (!is_empty(pq)) {
        QueueNode node = extract_min(pq);
        char* current = node.intersection;
        
        int current_index = get_index(graph, current);
        
        if (current_index == -1 || visited[current_index]) {
            free(current);
            continue;
        }

        visited[current_index] = 1;

        Node* neighbor = graph->nodes[current_index]->next;
        while (neighbor) {
            double adjusted_weight = adjust_weight(neighbor->distance, vehicle_priority);
            double alt = distances[current_index] + adjusted_weight;

            int neighbor_index = get_index(graph, neighbor->name);

            if (neighbor_index != -1 && alt < distances[neighbor_index]) {
                distances[neighbor_index] = alt;

                if (previous[neighbor_index]) {
                    free(previous[neighbor_index]);
                }
                
                previous[neighbor_index] = strdup(current);
                insert(pq, (QueueNode){strdup(neighbor->name), alt});
            }
            neighbor = neighbor->next;
        }
        
        free(current);
    }

    // Create path from previous
    Path* path = create_path();
    char* current = strdup(end);
    int current_index = get_index(graph, current);
    double total_distance = distances[current_index];

    //printf("ggggg%lfgggg \n", distances[2]);

    while (current != NULL) {
        path->nodes[path->length++] = strdup(current);
        
        if (strcmp(current, start) == 0) break;
        
        current = previous[current_index];
        if (current == NULL) break;
        
        current_index = get_index(graph, current);
    }

    
    for (int i = 0; i < path->length / 2; i++) {
        char* temp = path->nodes[i];
        path->nodes[i] = path->nodes[path->length - 1 - i];
        path->nodes[path->length - 1 - i] = temp;
    }

    path->total_distance = total_distance;

    
    free(distances);
    for (int i = 0; i < graph->size; i++) {
        free(previous[i]);
    }
    free(previous);
    free(visited);
    free(pq->nodes);
    free(pq);

    return path;
}


KShortestPaths* find_k_shortest_paths(Graph* graph, const char* start, const char* end, 
                                       int k, int vehicle_priority, NodeMapping* mappings, 
                                       int num_mappings, char* vehicle_type) {
    KShortestPaths* k_paths = create_k_shortest_paths(k);
    
    
    Path* shortest_path = dijkstra_single_path(graph, start, end, vehicle_priority, 
                                                mappings, num_mappings, vehicle_type);
    
    if (shortest_path == NULL) return k_paths;
    
    k_paths->paths[k_paths->num_paths++] = *shortest_path;
    free(shortest_path);

    
    for (int i = 1; i < k; i++) {
        for (int j = 0; j < k_paths->paths[i-1].length - 1; j++) {
            
            char* spur_node = k_paths->paths[i-1].nodes[j];
            Path* root_path = create_path();
            
            // Create root path up to spur node
            for (int m = 0; m <= j; m++) {
                root_path->nodes[root_path->length++] = strdup(k_paths->paths[i-1].nodes[m]);
            }

            // Remove links that are part of previous shortest paths
            for (int p = 0; p < k_paths->num_paths; p++) {
                Path* existing_path = &k_paths->paths[p];
                int match_length = 0;
                
                // Find matching prefix length
                for (int m = 0; m < j && m < existing_path->length; m++) {
                    if (strcmp(root_path->nodes[m], existing_path->nodes[m]) != 0) {
                        break;
                    }
                    match_length++;
                }

                // If prefix matches, remove the next link
                if (match_length == j + 1 && existing_path->length > match_length) {
                    remove_link(graph, existing_path->nodes[match_length-1], 
                                existing_path->nodes[match_length]);
                }
            }

            // Remove root path nodes from possible nodes in spur path
            remove_link(graph, spur_node, k_paths->paths[i-1].nodes[j+1]);

            // Find spur path
            Path* spur_path = dijkstra_single_path(graph, spur_node, end, 
                                                   vehicle_priority, mappings, 
                                                   num_mappings, vehicle_type);

            if (spur_path) {
                // Combine root and spur paths
                Path* candidate_path = create_path();
                for (int m = 0; m < root_path->length; m++) {
                    candidate_path->nodes[candidate_path->length++] = 
                        strdup(root_path->nodes[m]);
                }
                for (int m = 1; m < spur_path->length; m++) {
                    candidate_path->nodes[candidate_path->length++] = 
                        strdup(spur_path->nodes[m]);
                }
                
                // Restore removed links
                restore_link(graph, spur_node, k_paths->paths[i-1].nodes[j+1], 
                             spur_path->total_distance);

                candidate_path -> total_distance = spur_path -> total_distance;

                k_paths->paths[k_paths->num_paths++] = *candidate_path;
                free(candidate_path);  // Just free struct, not internal data
                free_path(spur_path);
            }

            // Clean up
            free_path(root_path);
            
            if (k_paths->num_paths >= k) break;
        }
        
        if (k_paths->num_paths >= k) break;
    }

    return k_paths;
}

// Utility functions for path management
Path* create_path() {
    Path* path = (Path*) malloc(sizeof(Path));
    path->nodes = (char**) malloc(MAX_NODES * sizeof(char*));
    path->length = 0;
    path->total_distance = 0.0;
    return path;
}

void free_path(Path* path) {
    if (path) {
        for (int i = 0; i < path->length; i++) {
            free(path->nodes[i]);
        }
        free(path->nodes);
        free(path);
    }
}

KShortestPaths* create_k_shortest_paths(int k) {
    KShortestPaths* k_paths = (KShortestPaths*) malloc(sizeof(KShortestPaths));
    k_paths->paths = (Path*) malloc(k * sizeof(Path));
    k_paths->num_paths = 0;
    k_paths->capacity = k;
    return k_paths;
}

void free_k_shortest_paths(KShortestPaths* k_paths) {
    if (k_paths) {
        for (int i = 0; i < k_paths->num_paths; i++) {
            Path* path = &k_paths->paths[i];
            for (int j = 0; j < path->length; j++) {
                free(path->nodes[j]);
            }
        }
        free(k_paths->paths);
        free(k_paths);
    }
}

void remove_link(Graph* graph, const char* from, const char* to) {
    Node* from_node = graph->nodes[get_index(graph, from)];
    Node* prev = NULL;
    Node* current = from_node->next;

    while (current) {
        if (strcmp(current->name, to) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                from_node->next = current->next;
            }
            free(current->name);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void restore_link(Graph* graph, const char* from, const char* to, double weight) {
    Node* from_node = graph->nodes[get_index(graph, from)];
    Node* new_node = (Node*) malloc(sizeof(Node));
    new_node->name = strdup(to);
    new_node->distance = weight;
    new_node->next = from_node->next;
    from_node->next = new_node;
}

void free_graph(Graph* graph) {
    for (int i = 0; i < graph->size; i++) {
        Node* current = graph->nodes[i];
        while (current != NULL) {
            Node* next = current->next;
            free(current->name);
            free(current);
            current = next;
        }
    }
    free(graph->nodes);
}

int readLocationsFromFile(const char *filename, Location locations[], int maxLocations) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    int count = 0;
    char line[300];
    while (fgets(line, sizeof(line), file) && count < maxLocations) {
        // Look for 'Name: ' and ' Address: ' to extract the name and address
        char *namePtr = strstr(line, "Name: ");
        char *addressPtr = strstr(line, " Address: ");

        if (namePtr && addressPtr) {
            // Extract name
            namePtr += 6; // Move past 'Name: '
            int nameLength = addressPtr - namePtr;
            strncpy(locations[count].name, namePtr, nameLength);
            locations[count].name[nameLength] = '\0';

            // Extract address
            addressPtr += 10; // Move past ' Address: '
            strncpy(locations[count].address, addressPtr, sizeof(locations[count].address) - 1);
            // Remove any trailing newline character from address
            locations[count].address[strcspn(locations[count].address, "\n")] = '\0';

            //printf("DEBUG: Loaded Location[%d]: Name='%s', Address='%s'\n", count, locations[count].name, locations[count].address);

            count++;
        } else {
            // Debugging: Line did not match expected format
            //printf("DEBUG: Skipping line due to format: %s\n", line);
        }
    }

    fclose(file);
    return count;
}

int getNodeIndexByName(const char *name, Location locations[], int numLocations) {
    for (int i = 0; i < numLocations; i++) {
        if (strcmp(locations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Return -1 if not found
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void free_resources(NodeMapping* node_mappings, Graph* graph, int num_nodes) {
    // Free node mappings
    for (int i = 0; i < num_nodes; i++) {
        free(node_mappings[i].identifier);
        free(node_mappings[i].name);
    }
    free(node_mappings);

    // Free graph memory
    free_graph(graph);  // Assumes you have a free_graph function to free the graph
}

void map_start_end_ids(const char* start_name, const char* end_name, NodeMapping* node_mappings, int num_nodes, const char** start_id, const char** end_id) {
    // Map start and end node names to their identifiers
    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(node_mappings[i].name, start_name) == 0) {
            *start_id = node_mappings[i].identifier;
        }
        if (strcmp(node_mappings[i].name, end_name) == 0) {
            *end_id = node_mappings[i].identifier;
        }
    }
}

int get_vehicle_priority(const char* vehicle_type) {
    // Define vehicle types and their corresponding priorities
    char vehicle_types[][MAX_VEHICLE_TYPE] = {"ambulance", "police", "bus", "car", "two_wheeler", "walking"};
    int priorities[] = {AMBULANCE_PRIORITY, POLICE_PRIORITY, BUS_PRIORITY, CAR_PRIORITY, TWO_WHEELER_PRIORITY, WALKING_PRIORITY};

    // Find and return the corresponding priority
    for (int i = 0; i < 6; i++) {
        if (strcmp(vehicle_type, vehicle_types[i]) == 0) {
            return priorities[i];
        }
    }
    return -1;  // Return -1 if the vehicle type is invalid
}

char *menu_mapping(int node){
    
    switch (node)
    {
    case 1:
        return "Mahatma Society _Node25_"; //Mahatma Society
        break;
    
    case 2:
        return "Shrikant Thackeray Path _Node17_"; //Shrikant Thackeray Path
        break;

    case 3:
        return "Left Bhusari Colony _Node40_"; //Left Bhusari Colony
        break;

    case 4:
        return "DP Road _Node45_"; //DP Road
        break;

    case 5:
        return "Indrayani CHS _Node66_"; //Indrayani CHS
        break;

    case 6:
        return "Kothrud _Node79_"; //Kothrud, Anandnagar
        break;

    case 7:
        return "Kothrud _Node85_"; //Kothrud, Pune City
        break;
    }

    return "Node101";
}

int main() {
   clock_t start_time = clock();

    // Open graph file
    FILE* file = fopen("graph2.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    int num_nodes, num_edges;
    fscanf(file, "%d %d", &num_nodes, &num_edges);

    // Allocate memory for node mappings and graph
    NodeMapping* node_mappings = (NodeMapping*) malloc(num_nodes * sizeof(NodeMapping));
    if (!node_mappings) exit(EXIT_FAILURE);

    Graph graph;
    graph.size = 0;
    graph.nodes = (Node**) malloc(num_nodes * sizeof(Node*));
    if (!graph.nodes) exit(EXIT_FAILURE);

    // Read nodes
    for (int i = 0; i < num_nodes; i++) {
        char identifier[20];
        char name[50];

        fscanf(file, " %s %s", identifier, name);
        add_node(&graph, identifier);

        node_mappings[i].identifier = strdup(identifier);
        node_mappings[i].name = strdup(name);
    }

    // Read edges
    for (int i = 0; i < num_edges; i++) {
        char from[20], to[20];
        double weight;
        fscanf(file, " %s %s %lf", from, to, &weight);
        add_edge(&graph, from, to, weight);
        add_edge(&graph, to, from, weight);
    }
    fclose(file);


    Location locations[100];
    int numLocations = readLocationsFromFile("node_names_and_addresses.txt", locations, 100);

    if (numLocations == 0) {
        printf("No locations were loaded.\n");
        free_resources(node_mappings, &graph, num_nodes);
        return 1;
    }


    char start_name[50], end_name[50], temp1[50], temp2[50];

    printf("**********************************************************************");
    printf("YEN'S ALGORITHM FOR MULTIPLE PATHS");
    printf("***********************************************************************");

    printf("List of areas to choose from:\n");
    printf("1.Mahatma Society\n");
    printf("2.Shrikant Thackeray Path\n");
    printf("3.Left Bhusari Colony\n");
    printf("4.DP Road\n");
    printf("5.Indrayani CHS\n");
    printf("6.Kothrud, Anandnagar\n");
    printf("7.Kothrud, Pune City\n");
    printf("************************************************************************\n");

    int start, end;
    char *tok;

    printf("Enter starting address number: ");
    scanf(" %d", &start);  
    printf("Enter ending address number: ");
    scanf(" %d", &end);   

    strcpy(start_name, menu_mapping(start));
    strcpy(end_name, menu_mapping(end));

    tok = strtok(start_name, "_");
    strcpy(temp1, tok);
    tok = strtok(NULL,"_");
    strcpy(start_name, tok);

    tok = strtok(end_name, "_");
    strcpy(temp2, tok);
    tok = strtok(NULL,"_");
    strcpy(end_name, tok);

    strcpy(temp1, "Mahatma Society ");

    int start_node_index = getNodeIndexByName(temp1, locations, numLocations);
    int end_node_index = getNodeIndexByName(temp2, locations, numLocations);

    printf("************************************************************************\n");
    printf("Starting Location: %s\n", temp1);
    printf("Starting Address: %s\n", locations[start_node_index].address);
    printf("Ending Location: %s\n", temp2);
    printf("Ending Address: %s\n", locations[end_node_index].address);
    printf("************************************************************************\n");


    char vehicle_type[20];
    printf("Enter vehicle type (ambulance, police, bus, car, two_wheeler, walking): ");
    scanf(" %19s", vehicle_type);
    printf("************************************************************************\n");

    // Find the vehicle priority based on input
    int vehicle_priority = get_vehicle_priority(vehicle_type);
    if (vehicle_priority == -1) {
        printf("Invalid vehicle type entered.\n");
        free_resources(node_mappings, &graph, num_nodes);
        return 1;
    }
    
    const char* start_id = NULL;
    const char* end_id = NULL;
    map_start_end_ids(start_name, end_name, node_mappings, num_nodes, &start_id, &end_id);

    if (!start_id || !end_id) {
        printf("Invalid start or end node entered.\n");
        free_resources(node_mappings, &graph, num_nodes);
        return 1;
    }

    
    int k = 3;  // Number of alternative paths to find
    KShortestPaths* k_paths = find_k_shortest_paths(&graph, start_id, end_id, k, 
                                                    vehicle_priority, node_mappings, 
                                                    num_nodes, vehicle_type);


    printf("\033[1;32mFound \033[1;31m%d \033[1;32malternative paths:\n", k_paths->num_paths);
    for (int i = 0; i < k_paths->num_paths; i++) {
        printf("\nPath %d:\n", i+1);
        for (int j = 0; j < k_paths->paths[i].length; j++) {
            printf("\033[1;33m%s ", get_node_name(node_mappings, num_nodes, k_paths->paths[i].nodes[j]));
        }
        printf("\n\033[1;31mTotal Distance: \033[1;32m%.3f km\n", k_paths->paths[i].total_distance / 1000.0);
    
    
        double time_taken = calculateTravelTime(k_paths->paths[i].total_distance, vehicle_type);
        int hours = (int)time_taken;
        int minutes = (int)((time_taken - hours) * 60);

        if (time_taken < 1.0) {
            minutes = (int)(time_taken * 60);
            hours = 0;
        }

        if (minutes == 0 && time_taken > 0) {
            minutes = 1;
        }
        
        printf("\033[1;31mTime Taken: \033[1;32m%d hr %d min\n", hours, minutes);
        
    }

    
    free_k_shortest_paths(k_paths);
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Execution time: %.3f seconds\n", time_spent);
    free_resources(node_mappings, &graph, num_nodes);
   

    return 0;
}
