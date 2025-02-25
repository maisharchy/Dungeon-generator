#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdbool.h>

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
#define MONSTER 'M'




typedef struct {
    int x, y, width, height;
} Room;
typedef struct {
    int x, y;
} Position;

int distance_map[HEIGHT][WIDTH]; 
char dungeon[HEIGHT][WIDTH];
Room rooms[MAX_ROOMS];
int room_count = 0;
char save_path[256];
int hardness[HEIGHT][WIDTH];

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
            if (dungeon[y1][x1] == ROCK){  
                dungeon[y1][x1] = CORRIDOR;
                hardness[y1][x1] = 0;
            }
            x1 += (x2 > x1) ? 1 : -1;
        }

        
        while (y1 != y2) {
            if (dungeon[y1][x1] == ROCK){  
                dungeon[y1][x1] = CORRIDOR;
                hardness[y1][x1] = 0;
            }y1 += (y2 > y1) ? 1 : -1;
        }
    }
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

void place_monster() {
    Position monster_start = {rooms[room_count-1].x + 1, rooms[room_count-1].y + 1};
    dijkstra_pathfinding(monster_start);
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

void print_hardness_map() {
    printf("\nHardness Map (Hexadecimal):\n");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%02X ", hardness[y][x]); 
        }
        printf("\n");
    }
}

void print_cost_grid() {
    printf("\nGrid Cost:\n");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (dungeon[y][x] == PLAYER) {
                printf(" @");
            } else {
                int cost = (dungeon[y][x] == ROOM || dungeon[y][x] == CORRIDOR) ? 1 : 3;
                printf("%2d", cost);
            }
        }
        printf("\n");
    }
}



int main(int argc, char *argv[]) {
    srand(time(NULL));
    setup_save_path();
    int load = 0, save = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) save = 1;
        if (strcmp(argv[i], "--load") == 0) load = 1;
    }
    
    Position player_pos = {0, 0};
    
    if (load) {
        load_dungeon();
        printf("\nDungeon:\n");
        print_dungeon();

        printf("\nHardness Map:\n");
        print_hardness_map();

        printf("\nNon-Tunneling Distance Map:\n");
        dijkstra(player_pos, false);
        print_distance_map(player_pos);

        printf("\nTunneling Distance Map:\n");
        dijkstra(player_pos, true);
        print_distance_map(player_pos);
        
    } else {
        init_dungeon();
        place_rooms();
        connect_rooms();
        place_staircases();
        place_player();
        place_monster();
        player_pos = (Position){rooms[0].x + 1, rooms[0].y + 1};  

    }
    printf("PC is at (y, x): %d, %d\n", player_pos.y, player_pos.x);
    printf("\nDungeon:\n");
    print_dungeon();

    printf("\nNon-Tunneling Distance Map:\n");
    dijkstra(player_pos, false);
    print_distance_map(player_pos);

    printf("\nTunneling Distance Map:\n");
    dijkstra(player_pos, true);
    print_distance_map(player_pos);

    printf("\nHardness Map:\n");
        print_hardness_map();
        print_cost_grid();
        

    if (save) save_dungeon();
    
    return 0;
}
