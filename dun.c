#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>  
#include <ncurses.h>
#include <termios.h>
#include <unistd.h>


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
    char symbol;
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
Monster monsters[100];  
int monster_count = 0;  
int num_monsters = 0;  
void handle_input();
void update_npcs();
void draw_dungeon();
void rest_turn();
void attempt_stairs_down();
void attempt_stairs_up();
void display_monster_list();
void scroll_up();
void scroll_down();
void move_pc(int dx, int dy, Position *player_pos);
void return_to_character_control();


void init_dungeon() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            dungeon[i][j] = ROCK;
            hardness[i][j] = (rand() % 254) + 1;  
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
        
    } else {
    }
    }
}



void move_monster(Monster *monster, Position player_pos) {
    
    dungeon[monster->y][monster->x] = ROOM;  

    if (monster->attributes & ATTRIBUTE_TELEPATHIC) {
        if (monster->x < player_pos.x) monster->x++;
        else if (monster->x > player_pos.x) monster->x--;

        if (monster->y < player_pos.y) monster->y++;
        else if (monster->y > player_pos.y) monster->y--;
    }
    else if (monster->attributes & ATTRIBUTE_SMART) {
        dijkstra(player_pos, (monster->attributes & ATTRIBUTE_TUNNEL) != 0);

        int min_dist = INT_MAX;
        int new_x = monster->x, new_y = monster->y;

        int dx[] = {0, 1, 0, -1};  
        int dy[] = {-1, 0, 1, 0};

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
    else if (monster->attributes & ATTRIBUTE_ERRATIC && rand() % 2 == 0) {
        int dx[] = {0, 1, 0, -1};  
        int dy[] = {-1, 0, 1, 0};
        int random_direction = rand() % 4;

        int nx = monster->x + dx[random_direction];
        int ny = monster->y + dy[random_direction];

        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && (dungeon[ny][nx] == ROOM || dungeon[ny][nx] == CORRIDOR)) {
            monster->x = nx;
            monster->y = ny;
        }
    }

    else {
        if (monster->x < player_pos.x) monster->x++;
        else if (monster->x > player_pos.x) monster->x--;

        if (monster->y < player_pos.y) monster->y++;
        else if (monster->y > player_pos.y) monster->y--;
    }

   
    char monster_symbol = MONSTER;  

    
    if (monster->attributes & ATTRIBUTE_MAGIC) {
        monster_symbol = 'M';
    } else if (monster->attributes & ATTRIBUTE_ERRATIC) {
        monster_symbol = 'E';
    } else if (monster->attributes & ATTRIBUTE_SMART) {
        monster_symbol = 'S';
    } else if (monster->attributes & ATTRIBUTE_TELEPATHIC) {
        monster_symbol = 'T';
    } else if (monster->attributes & ATTRIBUTE_TUNNEL) {
        monster_symbol = 'U';
    }

    
    
    if (dungeon[monster->y][monster->x] == ROOM || dungeon[monster->y][monster->x] == CORRIDOR) {
        dungeon[monster->y][monster->x] = monster_symbol;
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
        return (Event){-1, NULL};
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
        move_monster(event.monster, player_pos);  
        Event next_event = {event.turn + 1000 / event.monster->speed, event.monster};
        insert_event(event_queue, queue_size, next_event);
    }
}


bool check_player_death(Position player_pos) {
    for (int i = 0; i < monster_count; i++) {
        if (monsters[i].x == player_pos.x && monsters[i].y == player_pos.y) {
            return true; 
        }
    }
    return false;
}


void check_game_end(Position player_pos) {
    if (check_player_death(player_pos)) {
        printf("Game Over! The player has died.\n");
        exit(0);  
    }
    
}
void update_display(Position player_pos) {
    print_dungeon();
    
}

void game_loop(Position player_pos) {
    Event event_queue[1000];  
    int queue_size = 0;

    for (int i = 0; i < monster_count; i++) {
        Event event = {0, &monsters[i]};
        insert_event(event_queue, &queue_size, event);
    }

    while (true) {
        update_display(player_pos);
        
        handle_input(&player_pos);
        check_player_death(player_pos);  
        check_game_end(player_pos); 
        
        update_npcs(player_pos);    
        draw_dungeon();
        usleep(100000);  
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


void print_dungeon() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", dungeon[y][x]);
        }
        printf("\n");
    }
}


void place_monsters(int num_monsters) {
    for (int i = 0; i < num_monsters; i++) {
        place_monster(num_monsters);  
    }
}
void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(-1);  
}


void set_input_mode() {
    struct termios new_termios;

    tcgetattr(STDIN_FILENO, &new_termios);

    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1; 
    new_termios.c_cc[VTIME] = 0; 


    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

void reset_input_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &new_termios);
    new_termios.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

void handle_input(Position *player_pos) {
    char input;
    set_input_mode();

    printf("Move your character (@) using the following keys:\n");
    printf("  7, 8, 9 for diagonal movement (7=Up-Left, 8=Up, 9=Up-Right)\n");
    printf("  1, 2, 3 for diagonal movement (1=Down-Left, 2=Down, 3=Down-Right)\n");
    printf("  h for Left, k for Up, j for Down, l for Right\n");
    printf("  > to go down stairs, < to go up stairs\n");
    printf("  m to view monster list\n");
    printf("  q to quit\n");
    printf("Enter your choice: ");
    
    input = getchar();  
    
    switch(input) {
        case '7': 
        case 'y':
            move_pc(-1, -1, player_pos);
            break;
        case '8':
        case 'k':
            move_pc(0, -1, player_pos);
            break;
        case '9': 
        case 'u':
            move_pc(1, -1, player_pos);
            break;
        case '6': 
        case 'l':
            move_pc(1, 0, player_pos);
            break;
        case '3': 
        case 'n':
            move_pc(1, 1, player_pos);
            break;
        case '2': 
        case 'j':
            move_pc(0, 1, player_pos);
            break;
        case '1': 
        case 'b':
            move_pc(-1, 1, player_pos);
            break;
        case '4': 
        case 'h':
            move_pc(-1, 0, player_pos);
            break;
        case '>': 
            if (dungeon[player_pos->y][player_pos->x] == DOWN_STAIR) {
                attempt_stairs_down(player_pos);
            }
            break;
        case '<': 
            if (dungeon[player_pos->y][player_pos->x] == UP_STAIR) {
                attempt_stairs_up(player_pos);
            }
            break;
        case '5': 
        case ' ': 
        case '.':
            rest_turn();
            break;
        case 'm': 
           display_monster_list(player_pos);
           break;
        case 'Q': 
            endwin();
            exit(0);
            break;
        default:
            break;
    }

    reset_input_mode();

    update_npcs(*player_pos);
    draw_dungeon();
}

void move_pc(int dx, int dy, Position *player_pos) {
    int new_x = player_pos->x + dx;
    int new_y = player_pos->y + dy;


    if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
        if (dungeon[new_y][new_x] != ROCK) {
            dungeon[player_pos->y][player_pos->x] = ROOM;  
            player_pos->x = new_x;
            player_pos->y = new_y;
            dungeon[player_pos->y][player_pos->x] = PLAYER;  
        }
    }
}


void rest_turn() {
    mvprintw(0, 0, "You rest for a turn.");
    refresh();
}

void attempt_stairs_down(Position *player_pos) {
    mvprintw(0, 0, "You attempt to go down the stairs.");
    refresh();

    if (dungeon[player_pos->y][player_pos->x] == DOWN_STAIR) {
        init_dungeon(); 
        place_rooms();
        connect_rooms();
        place_stairs();
        place_monster(num_monsters);
        print_dungeon();
        draw_dungeon();

        player_pos->x = rooms[0].x + 1;
        player_pos->y = rooms[0].y + 1;
        place_player();
        game_loop(*player_pos);  
    }
}

void attempt_stairs_up(Position *player_pos) {
    mvprintw(0, 0, "You attempt to go up the stairs.");
    refresh();
    if (dungeon[player_pos->y][player_pos->x] == UP_STAIR) {
        init_dungeon();
        place_rooms();
        connect_rooms();
        place_stairs();
        place_monster(num_monsters);
        print_dungeon();
        draw_dungeon();

        player_pos->x = rooms[0].x + 1;
        player_pos->y = rooms[0].y + 1;
        place_player();
        game_loop(*player_pos);  
    }
}


#define MONSTER_LIST_SIZE 10  

int scroll_index = 0;  

void display_monster_list(Position *player_pos) {
    int ch;
    
    while (1) {
        system("clear");  

        printf("Monster List:\n");
        printf("Total Monsters: %d\n", monster_count);

        if (monster_count == 0) {
            printf("No monsters in the dungeon.\n");
        } else {
            int end_index = scroll_index + MONSTER_LIST_SIZE;
            if (end_index > monster_count) {
                end_index = monster_count;
            }

            for (int i = scroll_index; i < end_index; i++) {
                printf("Monster %d at (%d, %d), Speed: %d, Symbol: %c\n", 
                       i + 1, monsters[i].x, monsters[i].y, monsters[i].speed, monsters[i].symbol);
            }
        }

        printf("\nUse up and down arrow to scroll up/down, 'a' to return to the game.\n");
        
        ch = getchar(); 
        
        switch (ch) {
            case 'a':  
                clear();
                draw_dungeon();
                dungeon[player_pos->y][player_pos->x] = PLAYER;
                return;
            case KEY_UP:
                if (scroll_index > 0) scroll_index--;
                break;
            case KEY_DOWN:
                if (scroll_index + MONSTER_LIST_SIZE < monster_count) scroll_index++;
                break;
        }
    }
}


void scroll_up(Position *player_pos) {
    if (scroll_index > 0) {
        scroll_index--;  
        display_monster_list(player_pos);  
    }
}

void scroll_down(Position *player_pos) {
    if (scroll_index + MONSTER_LIST_SIZE < monster_count) {
        scroll_index++; 
        display_monster_list(player_pos);
    }
}
void update_npcs(Position player_pos) {
    for (int i = 0; i < monster_count; i++) {
        move_monster(&monsters[i], player_pos);
    }
}
void draw_dungeon() {
    clear();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvaddch(i, j, dungeon[i][j]);  
        }
    }
    refresh();
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
    
    init_dungeon();
    place_rooms();
    connect_rooms();
    place_stairs();
    place_monster(num_monsters);
    //print_dungeon(); 
    draw_dungeon();  


    
    Position player_pos = {rooms[0].x + 1, rooms[0].y + 1};
    place_player();
    game_loop(player_pos);

    return 0;
}