#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>  // For usleep

#define WIDTH 80
#define HEIGHT 21
#define MIN_ROOMS 6
#define MAX_ROOMS 10
#define MIN_ROOM_WIDTH 4
#define MIN_ROOM_HEIGHT 3
#define ROCK ' '
#define ROOM '.'
#define CORRIDOR '#'
#define UP_STAIR '<'
#define DOWN_STAIR '>'
#define PLAYER '@'
#define MONSTER 'm'
#define DEFAULT_MONSTER_COUNT 10  
#define ATTRIBUTE_SMART      0x00000001
#define ATTRIBUTE_TELEPATHIC 0x00000002
#define ATTRIBUTE_TUNNEL     0x00000004
#define ATTRIBUTE_ERRATIC    0x00000008
#define ATTRIBUTE_MAGIC      0x00000010




typedef struct {
    int x, y, width, height;
} Room;

typedef struct {
    int x, y;
} Position;

typedef struct {
    int x, y;
    int speed;
    int attributes;  // Bitmask for monster attributes
} Monster;
void print_dungeon();
void print_cost_grid();
void print_distance_map(Position player);
int distance_map[HEIGHT][WIDTH]; 
char dungeon[HEIGHT][WIDTH];
Room rooms[MAX_ROOMS];
int room_count = 0;
char save_path[256];
int hardness[HEIGHT][WIDTH];
Monster monsters[100];  // Array to store monsters
int monster_count = 0;  // Number of monsters
int num_monsters = 0;  // Default monster count



void init_dungeon() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            dungeon[i][j] = ROCK;
            hardness[i][j] = (rand() % 254) + 1;  // Random hardness (1-254) for rocks
        }
    }
}

int is_valid_room(int x, int y, int w, int h) {
    if (x + w >= WIDTH - 1 || y + h >= HEIGHT - 1) return 0;
    for (int i = y - 1; i <= y + h; i++) {
        for (int j = x - 1; j <= x + w; j++) {
            if (i >= 0 && j >= 0 && i < HEIGHT && j < WIDTH && dungeon[i][j] != ROCK) {
                return 0;
            }
        }
    }
    return 1;
}

void place_rooms() {
    while (room_count < MIN_ROOMS) {
        int w = MIN_ROOM_WIDTH + rand() % 6;
        int h = MIN_ROOM_HEIGHT + rand() % 4;
        int x = 1 + rand() % (WIDTH - w - 2);
        int y = 1 + rand() % (HEIGHT - h - 2);
        
        if (is_valid_room(x, y, w, h)) {
            for (int i = y; i < y + h; i++) {
                for (int j = x; j < x + w; j++) {
                    dungeon[i][j] = ROOM;
                    hardness[i][j] = 0;
                }
            }
            rooms[room_count++] = (Room){x, y, w, h};
        }
    }
}

void connect_rooms() {
    for (int i = 1; i < room_count; i++) {
        Room a = rooms[i - 1], b = rooms[i];
        int x1 = a.x + a.width / 2, y1 = a.y + a.height / 2;
        int x2 = b.x + b.width / 2, y2 = b.y + b.height / 2;

        while (x1 != x2) {
            if (dungeon[y1][x1] == ROCK) {  
               dungeon[y1][x1] = CORRIDOR;
               hardness[y1][x1] = 0;
            }
            x1 += (x2 > x1) ? 1 : -1;
        }

        while (y1 != y2) {
            if (dungeon[y1][x1] == ROCK) {  
                dungeon[y1][x1] = CORRIDOR;
                hardness[y1][x1] = 0;
            }  
            y1 += (y2 > y1) ? 1 : -1;
        }

    }
}
void place_stairs() {
    if (room_count < 2) {
        printf("Not enough rooms for staircases.\n");
        return;
    }

    int up_room_idx = rand() % room_count;
    Room up_room = rooms[up_room_idx];
    int up_x = up_room.x + 1 + rand() % (up_room.width - 2);
    int up_y = up_room.y + 1 + rand() % (up_room.height - 2);
    dungeon[up_y][up_x] = UP_STAIR;

    int down_room_idx;
    do {
        down_room_idx = rand() % room_count;
    } while (down_room_idx == up_room_idx); 

    Room down_room = rooms[down_room_idx];
    int down_x = down_room.x + 1 + rand() % (down_room.width - 2);
    int down_y = down_room.y + 1 + rand() % (down_room.height - 2);
    dungeon[down_y][down_x] = DOWN_STAIR;
}

void dijkstra(Position player, bool tunneling) {
    int visited[HEIGHT][WIDTH] = {0};
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            distance_map[i][j] = INT_MAX; 
        }
    }

    distance_map[player.y][player.x] = 0;

    for (int iter = 0; iter < WIDTH * HEIGHT; iter++) {
        int min_dist = INT_MAX, min_x = -1, min_y = -1;
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (!visited[i][j] && distance_map[i][j] < min_dist) {
                    min_dist = distance_map[i][j];
                    min_x = j;
                    min_y = i;
                }
            }
        }

        if (min_x == -1) break;
        visited[min_y][min_x] = 1;

        int dx[] = {0, 1, 0, -1, -1, -1, 1, 1};  
        int dy[] = {-1, 0, 1, 0, -1, 1, -1, 1};

        for (int d = 0; d < 8; d++) {  
            int nx = min_x + dx[d], ny = min_y + dy[d];

            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                if (!tunneling && dungeon[ny][nx] == ROCK) continue;

                int move_cost = (tunneling) ? (hardness[ny][nx] / 85) + 1 : 1;
                int new_cost = distance_map[min_y][min_x] + move_cost;

                if (new_cost < distance_map[ny][nx]) {
                    distance_map[ny][nx] = new_cost;
                }
            }
        }
    }
}


void place_monster(int num_monsters) {
    
    if (room_count < 1) {
        return;
    }

    for (int i = 0; i < num_monsters; i++) {
    int room_idx = rand() % room_count;
    Room selected_room = rooms[room_idx];
    
    Position monster_start = { 
        .x = selected_room.x + 1 + rand() % (selected_room.width - 2),  // Random X within room
        .y = selected_room.y + 1 + rand() % (selected_room.height - 2)  // Random Y within room
    };
    
    Monster new_monster = {
        .x = monster_start.x,
        .y = monster_start.y,
        .speed = rand() % 10 + 1,  // Random speed
        .attributes = rand() % 32  // Random attributes
    };
    
    monsters[monster_count++] = new_monster; // Add monster to array
    char monster_symbol = MONSTER;  // Default symbol
    if (new_monster.attributes & ATTRIBUTE_MAGIC) {
        monster_symbol = 'M'; // Magic takes precedence
    } else if (new_monster.attributes & ATTRIBUTE_ERRATIC) {
        monster_symbol = 'E'; // Erratic takes precedence
    } else if (new_monster.attributes & ATTRIBUTE_SMART) {
        monster_symbol = 'S'; // Smart monsters
    } else if (new_monster.attributes & ATTRIBUTE_TELEPATHIC) {
        monster_symbol = 'T'; // Telepathic monsters
    } else if (new_monster.attributes & ATTRIBUTE_TUNNEL) {
        monster_symbol = 'U'; // Tunnel monsters
    }

    if (dungeon[new_monster.y][new_monster.x] == ROOM || dungeon[new_monster.y][new_monster.x] == CORRIDOR) {
        dungeon[new_monster.y][new_monster.x] = monster_symbol;
        printf("Monster placed at (%d, %d) with speed %d and attributes 0x%X, symbol: %c\n",
               new_monster.x, new_monster.y, new_monster.speed, new_monster.attributes, monster_symbol);
    } else {
    }
    }
}



void move_monster(Monster *monster, Position player_pos) {
    
    dungeon[monster->y][monster->x] = ROOM;  // Reset previous position to room type or empty space

    // Handle Telepathic Monsters
    if (monster->attributes & ATTRIBUTE_TELEPATHIC) {
        // Telepathic monsters always know where the PC is
        if (monster->x < player_pos.x) monster->x++;
        else if (monster->x > player_pos.x) monster->x--;

        if (monster->y < player_pos.y) monster->y++;
        else if (monster->y > player_pos.y) monster->y--;
    }
    // Handle Intelligent Monsters (Smart)
    else if (monster->attributes & ATTRIBUTE_SMART) {
        // Used Dijkstra's pathfinding to find the shortest path to the player
        dijkstra(player_pos, (monster->attributes & ATTRIBUTE_TUNNEL) != 0);

        // Move the monster to the next position towards the player 
        int min_dist = INT_MAX;
        int new_x = monster->x, new_y = monster->y;

        int dx[] = {0, 1, 0, -1};  // 4 directions: up, right, down, left
        int dy[] = {-1, 0, 1, 0};

        // Find the best direction to move
        for (int i = 0; i < 4; i++) {
            int nx = monster->x + dx[i];
            int ny = monster->y + dy[i];

            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && distance_map[ny][nx] < min_dist) {
                min_dist = distance_map[ny][nx];
                new_x = nx;
                new_y = ny;
            }
        }

        monster->x = new_x;
        monster->y = new_y;
    }
    // Handle Erratic Monsters
    else if (monster->attributes & ATTRIBUTE_ERRATIC && rand() % 2 == 0) {
        // Erratic monsters have a 50% chance of moving randomly
        int dx[] = {0, 1, 0, -1};  // 4 directions
        int dy[] = {-1, 0, 1, 0};
        int random_direction = rand() % 4;

        int nx = monster->x + dx[random_direction];
        int ny = monster->y + dy[random_direction];

        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && (dungeon[ny][nx] == ROOM || dungeon[ny][nx] == CORRIDOR)) {
            monster->x = nx;
            monster->y = ny;
        }
    }
    // Handle Non-Intelligent Monsters (Basic movement)
    else {
        // Move towards the player
        if (monster->x < player_pos.x) monster->x++;
        else if (monster->x > player_pos.x) monster->x--;

        if (monster->y < player_pos.y) monster->y++;
        else if (monster->y > player_pos.y) monster->y--;
    }

   
    char monster_symbol = MONSTER;  // Default symbol

    
    if (monster->attributes & ATTRIBUTE_MAGIC) {
        monster_symbol = 'M'; // Magic takes precedence
    } else if (monster->attributes & ATTRIBUTE_ERRATIC) {
        monster_symbol = 'E'; // Erratic takes precedence
    } else if (monster->attributes & ATTRIBUTE_SMART) {
        monster_symbol = 'S'; // Smart monsters
    } else if (monster->attributes & ATTRIBUTE_TELEPATHIC) {
        monster_symbol = 'T'; // Telepathic monsters
    } else if (monster->attributes & ATTRIBUTE_TUNNEL) {
        monster_symbol = 'U'; // Tunnel monsters
    }

    
    
    if (dungeon[monster->y][monster->x] == ROOM || dungeon[monster->y][monster->x] == CORRIDOR) {
        dungeon[monster->y][monster->x] = monster_symbol;  // Using the monster's symbol
    } else {
        monster->x = monster->x;
        monster->y = monster->y;
    }
}


typedef struct {
    int turn;
    Monster *monster;
} Event;

 
void insert_event(Event *queue, int *queue_size, Event event) {
    queue[*queue_size] = event;
    (*queue_size)++;
}

Event dequeue_event(Event *queue, int *queue_size) {
    if (*queue_size == 0) {
        return (Event){-1, NULL};  // No events
    }
    Event event = queue[0];
    for (int i = 0; i < *queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    (*queue_size)--;
    return event;
}


void simulate_turn(Event *event_queue, int *queue_size, Position player_pos) {
    Event event = dequeue_event(event_queue, queue_size);
    if (event.monster != NULL) {
        move_monster(event.monster, player_pos);  // Move the monster
        // Adding the next move event back into the queue
        Event next_event = {event.turn + 1000 / event.monster->speed, event.monster};
        insert_event(event_queue, queue_size, next_event);
    }
}


bool check_player_death(Position player_pos) {
    for (int i = 0; i < monster_count; i++) {
        if (monsters[i].x == player_pos.x && monsters[i].y == player_pos.y) {
            return true;  // Player is dead
        }
    }
    return false;
}


void check_game_end(Position player_pos) {
    if (check_player_death(player_pos)) {
        printf("Game Over! The player has died.\n");
        exit(0);  // Game ends
    }
    
}
void update_display(Position player_pos) {
    print_dungeon();
    
}

void game_loop(Position player_pos) {
    Event event_queue[1000];  // Priority queue for monster events
    int queue_size = 0;

    // Adding monster move events to the queue
    for (int i = 0; i < monster_count; i++) {
        Event event = {0, &monsters[i]};
        insert_event(event_queue, &queue_size, event);
    }

    while (true) {
        update_display(player_pos);
        usleep(250000);  

        simulate_turn(event_queue, &queue_size, player_pos);  

        check_game_end(player_pos);  
    }
}


void print_distance_map(Position player) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == player.x && y == player.y) {
                printf("@"); 
            } else {
                if (distance_map[y][x] == INT_MAX) {
                    printf(" "); 
                } else {
                    printf("%d", distance_map[y][x] % 10);  
                }
            }
        }
        printf("\n");
    }
}


void dijkstra_pathfinding(Position start) {
    int dist[HEIGHT][WIDTH];

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            dist[i][j] = INT_MAX;
        }
    }

    dist[start.y][start.x] = 0;

    for (int iteration = 0; iteration < WIDTH * HEIGHT; iteration++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (dungeon[y][x] != ROCK) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if ((dx == 0 || dy == 0 || abs(dx) == abs(dy)) &&  
                                (x + dx >= 0 && x + dx < WIDTH && y + dy >= 0 && y + dy < HEIGHT)) {
                                int new_dist = dist[y][x] + 1;  
                                if (new_dist < dist[y + dy][x + dx]) {
                                    dist[y + dy][x + dx] = new_dist;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    dungeon[start.y][start.x] = MONSTER; 
}


void place_staircases() {
    int up_index = rand() % room_count;
    int down_index;
    do { down_index = rand() % room_count; } while (down_index == up_index);
    
    dungeon[rooms[up_index].y + rooms[up_index].height / 2][rooms[up_index].x + rooms[up_index].width / 2] = UP_STAIR;
    dungeon[rooms[down_index].y + rooms[down_index].height / 2][rooms[down_index].x + rooms[down_index].width / 2] = DOWN_STAIR;
}

void place_player() {
    dungeon[rooms[0].y + 1][rooms[0].x + 1] = PLAYER;
}

void save_dungeon() {
    FILE *file = fopen(save_path, "wb");
    if (!file) return;

    fwrite("RLG327", 1, 6, file);
    uint16_t version = htons(1);
    fwrite(&version, sizeof(version), 1, file);

    uint32_t rc = htonl(room_count);
    fwrite(&rc, sizeof(rc), 1, file);

    for (int i = 0; i < room_count; i++) {
        uint8_t room_data[4] = {rooms[i].x, rooms[i].y, rooms[i].width, rooms[i].height};
        fwrite(room_data, 1, 4, file);
    }

    fwrite(dungeon, sizeof(dungeon), 1, file);
    fclose(file);
}

void load_dungeon() {
    FILE *file = fopen(save_path, "rb");
    if (!file) return;

    char magic[6];
    fread(magic, 1, 6, file);
    if (strncmp(magic, "RLG327", 6) != 0) {
        fclose(file);
        return;
    }

    uint16_t version;
    fread(&version, sizeof(version), 1, file);
    version = ntohs(version);

    uint32_t rc;
    fread(&rc, sizeof(rc), 1, file);
    room_count = ntohl(rc);

    for (int i = 0; i < room_count; i++) {
        uint8_t room_data[4];
        fread(room_data, 1, 4, file);
        rooms[i] = (Room){room_data[0], room_data[1], room_data[2], room_data[3]};
    }

    fread(dungeon, sizeof(dungeon), 1, file);
    fclose(file);
}

void print_dungeon() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", dungeon[y][x]);
        }
        printf("\n");
    }
}

void setup_save_path() {
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "Error: HOME environment variable not found.\n");
        return;
    }

    
    snprintf(save_path, sizeof(save_path), "%s/.rlg327", home);

    
    struct stat st = {0};
    if (stat(save_path, &st) == -1) {
        if (mkdir(save_path, 0755) == -1) {
            perror("Error creating .rlg327 directory");
            return;
        }
    }

    
    snprintf(save_path, sizeof(save_path), "%s/.rlg327/dungeon", home);
}

/*
void handle_nummon(int nummon) {
    // Ensure the requested number of monsters is within a reasonable range
    if (nummon <= 0) {
        printf("Please specify a positive number of monsters.\n");
        return;
    }

    // Place the specified number of monsters
    for (int i = 0; i < nummon; i++) {
        place_monster(num_monsters);
    }
}
*/


void place_monsters(int num_monsters) {
    for (int i = 0; i < num_monsters; i++) {
        place_monster(num_monsters);  
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    int num_monsters = DEFAULT_MONSTER_COUNT;  

    
    for (int i = 1; i < argc; i++) {
       if (strcmp(argv[i], "--nummon") == 0 && i + 1 < argc) {
           num_monsters = atoi(argv[i + 1]);  
           i++;  
        }
        
    }
    
    //place_monsters(num_monsters);
    init_dungeon();
    place_rooms();
    place_stairs();
    connect_rooms();
    place_monster(num_monsters);
    
    Position player_pos = {rooms[0].x + 1, rooms[0].y + 1};
    place_player();
    game_loop(player_pos);

    return 0;
}