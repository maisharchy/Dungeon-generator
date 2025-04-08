# Dungeon Generator

This project is a simple dungeon generator for a roguelike game. It creates a dungeon with randomly placed rooms and corridors, adds stairs and monsters, and provides features to save and load dungeons,(removed this time for better quality) find shortest path to PC. Generates monsters randomly with randomized attributes, kills the PC.
My program, parser.cpp, reads the file $HOME/.rlg327/monster_desc.txt and parses monster templates. It iterates through the file, extracts valid monster fields, and stores them in a MonsterDescription class. Then it copies the data into a vector<Monsterr> monsters list. Once all monsters are read, the program prints them to the screen and exits.

## Features Implemented:

- Initializes a dungeon with rock cells.
- Randomly generates non-overlapping rooms.
- Connects rooms using corridors.
- Places at least one upward and one downward staircase.
- Ensures full connectivity for player movement.
- Prints the dungeon layout to standard output.
- Saves and loads a dungeon to/from a binary file.
- Dijkstra's pathfinding for tunnelling and non-tunnelling mosters
- Move monster and place monster features implemented along with game loop logic.
- Handles user input and pc movement
- Manages game rendering and display
- Handles NPC movement and AI behavior
- Displays Monster list
- Up and Down stair logic
- Implements fog of war for a more immersive experience.
- Introduces a terrain map for improved visualization.
 

## Function Overview:

### `init_dungeon()`
Initializes the dungeon by filling it with rock cells (`' '`).

### `is_valid_room(int x, int y, int w, int h)`
Checks if a room can be placed at a given location without overlapping with existing rooms.

### `place_rooms()`
Randomly generates and places a minimum number of valid rooms in the dungeon.

### `connect_rooms()`
Connects rooms using corridors by drawing horizontal and vertical paths between them.

### `place_staircases()`
Places an upward and a downward staircase in different rooms.

### `place_player()`
Places the player character (`@`) in the first room.

### `save_dungeon()`
Saves the dungeon structure and room data to a binary file.

### `load_dungeon()`
Loads the dungeon structure and room data from a saved binary file.

### `setup_save_path()`
Sets up the save file path in the user's home directory (`~/.rlg327/dungeon`).

### `print_dungeon()`
Prints the dungeon layout to the console.


### `print_distance_map(Position player)`
Prints a distance cost map based on the player's position.

### `dijkstra(Position player, bool tunneling)`
Generates a distance map based on the Dijkstra algorithm. Supports tunneling (moving through rocks with a cost).

### `dijkstra_pathfinding(Position start)`
Generates a path for monsters to follow using Dijkstra’s algorithm.

### `place_monster()`
Randomly places a specified number of monsters within valid rooms in the dungeon, assigning them random attributes and symbols. It updates the dungeon grid with the monster's position and details, ensuring placement in accessible areas.

### `move_monster()`
moves monsters based on their attributes, such as telepathic, smart, erratic, or basic movement towards the player. It updates their position on the dungeon grid and ensures the movement is valid.

### `insert_event`  
Inserts an event into the priority queue and increments the queue size.

### `dequeue_event`  
Dequeues and returns the highest priority event from the queue, adjusting the queue size.

### `simulate_turn`  
Simulates a monster's turn by moving it based on its attributes, then re-queues the monster's next movement event.

### `check_player_death`  
Checks if the player has been killed by any monsters at the player's current position.

### `check_game_end`  
Ends the game if the player is dead, printing a "Game Over" message.

### `update_display`  
Updates and prints the current dungeon state, including player position and any other relevant data.

### `game_loop`  
Main game loop that handles event processing, monster movements, and checks for game-ending conditions.

### `void init_ncurses()`  
- Initializes the ncurses library for handling input and output.  

### `void set_input_mode()`  
- Sets terminal input mode to disable canonical mode and echo.  

### `void reset_input_mode()`  
- Restores the terminal input mode to its original settings.  

### `void handle_input(Position *player_pos)`  
- Handles user input for character movement and interactions.  

### `void move_pc(int dx, int dy, Position *player_pos)`  
- Moves the player character within the dungeon if no obstacles exist.  

### `void rest_turn()`  
- Lets the player rest for a turn.  

### `void attempt_stairs_down(Position *player_pos)`  
- Moves the player down a staircase if standing on one.  

### `void attempt_stairs_up(Position *player_pos)`  
- Moves the player up a staircase if standing on one.  

### `void display_monster_list(Position *player_pos)`  
- Displays a scrollable list of monsters in the dungeon.  

### `void scroll_up(Position *player_pos)`  
- Scrolls up in the monster list.  

### `void scroll_down(Position *player_pos)`  
- Scrolls down in the monster list.  

### `void update_npcs(Position player_pos)`  
- Updates the positions of non-player characters (monsters).  

### `void draw_dungeon()`  
- Renders the dungeon and its elements on the screen.  
### `init_seen()`
Initializes all tiles in the dungeon as unseen.

### `update_seen(Position player_pos)`
Updates the visibility of tiles around the player based on a radius.

### `render_dungeon(Position player_pos)`
Renders the dungeon, displaying visible tiles and handling fog of war effects.

### `update_visible_area()`
Increases the player's visibility range up to the MAX_VISIBILITY limit.

### `Dice`
- Parses and formats dice strings like 10+2d4.

### `Monsterr`
- Struct that holds finalized monster data (1 color, 1 ability, stringified dice).

### `MonsterDescription`
- Used during parsing to collect all monster fields including multiple values.

### `split()`
- Splits a line into words (used for color/abilities).

### `parse_monsters()`
- Reads monster file, parses each valid entry into a Monsterr, skips invalid ones.

### `save_monsters()`
- Writes all Monsterr entries to a file in the correct format.


## Command-Line Arguments:

- `--save` - Saves the generated dungeon to a file.
- `--load` - Loads a previously saved dungeon from a file.
- `--load --save` - Loads an existing dungeon and saves it again (for verification).
- `load` - Loads a dungeon without making any changes (no save).

## Command-Line Arguments:
### `nummon` switch

The `--nummon <n>` argument allows you to specify how many monsters should be generated when the dungeon is created. For example, if you run:
./dun --nummon 10

## NEW 1.07 instructions and comments:


I have implemented monster_parser.cpp monster_parser.h which is independent from the dun.cpp project and have also commented out the other functionalities of the main as we are supposed to parse and print out monster description for the project. I have also done the optional item parser coding as we need to work on it for next assignments. 
My program handles the very basic error checking that was required. I did NOT implement any further error checking, as I'm under the impression that we don't have too.