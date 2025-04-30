# Chess Game in C++

A simple console-based Chess game implemented in C++ with basic functionalities like moving pieces, checking for check and checkmate, and alternating turns between two bots.


## Overview

This is a simplified implementation of Chess in C++. The game supports basic chess rules such as piece movement, check, checkmate, and turn alternation. It includes a `Board` class for managing the board and pieces, a `ChessGame` class to handle the game logic, and functionality for random moves for a bot opponent.



## How to Play

1. **Starting the Game:**
   - Run the program to start the game.
   - The game will automatically start with the White player.
   - The board will be printed in the console with the pieces set up in their starting positions.

2. **Making Moves:**
   - The game will alternate between the two players. Each player takes one turn at a time.
   - Player's moves are made randomly by the bot, and you can simulate the next move by pressing Enter.

3. **Game End:**
   - The game ends if a player wins by checkmate or the game ends in a draw (stalemate).
   - After each checkmate or stalemate detection, the game announces the result.

4. **Press Enter:**
   - After each move, press Enter to continue to the next turn.

