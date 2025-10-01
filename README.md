# Dungeon Generator

This project is a simple dungeon generator for a roguelike game. It creates a dungeon with randomly placed rooms and corridors, adds stairs and monsters, and provides features to save and load dungeons,(removed this time for better quality) find shortest path to PC. Generates monsters randomly with randomized attributes, kills the PC.

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


## Command-Line Arguments:

- `--save` - Saves the generated dungeon to a file.
- `--load` - Loads a previously saved dungeon from a file.
- `--load --save` - Loads an existing dungeon and saves it again (for verification).
- `load` - Loads a dungeon without making any changes (no save).

## NEW 1.04 instructions and comments:

## Command-Line Arguments:
### `nummon` switch

The `--nummon <n>` argument allows you to specify how many monsters should be generated when the dungeon is created. For example, if you run:


./dun --nummon 10

