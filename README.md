# Dungeon Generator

This project is a simple dungeon generator for a roguelike game. It creates a dungeon with randomly placed rooms and corridors, adds stairs and monsters, and provides features to save and load dungeons, find shortest path to PC.

## Features Implemented:

- Initializes a dungeon with rock cells.
- Randomly generates non-overlapping rooms.
- Connects rooms using corridors.
- Places at least one upward and one downward staircase.
- Ensures full connectivity for player movement.
- Prints the dungeon layout to standard output.
- Saves and loads a dungeon to/from a binary file.
- Dijkstra's pathfinding for tunnelling and non-tunnelling mosters

 

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

### `print_hardness_map()`
Prints the hardness map (randomly generated hardness values for each cell).

### `print_cost_grid()`
Prints a grid representing the terrain cost (based on dungeon cells).

### `print_distance_cost_map(Position player)`
Prints a distance cost map based on the player's position.

### `dijkstra(Position player, bool tunneling)`
Generates a distance map based on the Dijkstra algorithm. Supports tunneling (moving through rocks with a cost).

### `dijkstra_pathfinding(Position start)`
Generates a path for monsters to follow using Dijkstra’s algorithm.

### `place_monster()`
Places a monster (`M`) at a random valid position using the Dijkstra pathfinding.[Needs farther improvement for 1.04 I still haven'ts started 1.04. Work in progress]

## Command-Line Arguments:

- `--save` - Saves the generated dungeon to a file.
- `--load` - Loads a previously saved dungeon from a file.
- `--load --save` - Loads an existing dungeon and saves it again (for verification).
- `load` - Loads a dungeon without making any changes (no save).

## NEW 1.03 instructions and comments:

## How to get the maps:

- simply use ./dun after the make command.
[the load and save for 1.02 has not been updated for this 1.03 project. The professer said in class we don't have to.]

## New this time:

I have implemented the distance map using dijkstra's algorithm, My code generates a dungeon, prints non-tunneling, tunneling distance map, hardness map in hexadecimal, and a grid-cost map. Last two maps I have added because it was in the test cases, but not in the instructions.