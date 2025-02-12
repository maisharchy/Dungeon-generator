#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>

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

typedef struct {
    int x, y, width, height;
} Room;

char dungeon[HEIGHT][WIDTH];
Room rooms[MAX_ROOMS];
int room_count = 0;
char save_path[256];

void init_dungeon() {
    memset(dungeon, ROCK, sizeof(dungeon));
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
            if (dungeon[y1][x1] == ROCK)  
                dungeon[y1][x1] = CORRIDOR;
            x1 += (x2 > x1) ? 1 : -1;
        }

        
        while (y1 != y2) {
            if (dungeon[y1][x1] == ROCK)  
                dungeon[y1][x1] = CORRIDOR;
            y1 += (y2 > y1) ? 1 : -1;
        }
    }
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


int main(int argc, char *argv[]) {
    srand(time(NULL));
    setup_save_path();
    int load = 0, save = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) save = 1;
        if (strcmp(argv[i], "--load") == 0) load = 1;
    }
    
    if (load) {
        load_dungeon();
    } else {
        init_dungeon();
        place_rooms();
        connect_rooms();
        place_staircases();
        place_player();
    }
    print_dungeon();
    if (save) save_dungeon();
    
    return 0;
}
