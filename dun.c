#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 21
#define MIN_ROOMS 6
#define MIN_ROOM_WIDTH 4
#define MIN_ROOM_HEIGHT 3
#define MAX_ROOMS 10

char dungeon[HEIGHT][WIDTH];

typedef struct {
    int x, y, w, h;
} Room;

Room rooms[MAX_ROOMS];
int room_count = 0;

void init_dungeon() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                dungeon[i][j] = ' ';  // Immutable border
            } else {
                dungeon[i][j] = ' ';
            }
        }
    }
}

int is_valid_room(int x, int y, int w, int h) {
    if (x + w >= WIDTH - 1 || y + h >= HEIGHT - 1) return 0;
    for (int i = y - 1; i < y + h + 1; i++) {
        for (int j = x - 1; j < x + w + 1; j++) {
            if (dungeon[i][j] != ' ') return 0;
        }
    }
    return 1;
}

void place_rooms() {
    srand(time(NULL));
    while (room_count < MIN_ROOMS) {
        int w = MIN_ROOM_WIDTH + rand() % 6;
        int h = MIN_ROOM_HEIGHT + rand() % 4;
        int x = 1 + rand() % (WIDTH - w - 2);
        int y = 1 + rand() % (HEIGHT - h - 2);

        if (is_valid_room(x, y, w, h)) {
            rooms[room_count++] = (Room){x, y, w, h};
            for (int i = y; i < y + h; i++) {
                for (int j = x; j < x + w; j++) {
                    dungeon[i][j] = '.';
                }
            }
        }
    }
}

void connect_rooms() {
    for (int i = 1; i < room_count; i++) {
        int x1 = rooms[i - 1].x + rooms[i - 1].w / 2;
        int y1 = rooms[i - 1].y + rooms[i - 1].h / 2;
        int x2 = rooms[i].x + rooms[i].w / 2;
        int y2 = rooms[i].y + rooms[i].h / 2;

        while (x1 != x2) {
            dungeon[y1][x1] = '#';
            x1 += (x2 > x1) ? 1 : -1;
        }
        while (y1 != y2) {
            dungeon[y1][x1] = '#';
            y1 += (y2 > y1) ? 1 : -1;
        }
    }
}

void place_stairs() {
    int r = rand() % room_count;
    dungeon[rooms[r].y + 1][rooms[r].x + 1] = '<';
    r = rand() % room_count;
    dungeon[rooms[r].y + 1][rooms[r].x + 1] = '>';
}

void print_dungeon() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", dungeon[i][j]);
        }
        printf("\n");
    }
}

int main() {
    init_dungeon();
    place_rooms();
    connect_rooms();
    place_stairs();
    print_dungeon();
    return 0;
}
