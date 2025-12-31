<div align ="center">
    <!--LOGO-->
    <a href="github.com/dventurb/tictactoe">
    <img src="https://raw.githubusercontent.com/dventurb/tictactoe/refs/heads/main/assets/icon.png" alt="Logo" width="64" height="64">
</a>

<!--PROJECT NAME-->
<h1>Tic Tac Toe</h1>

![screenshot](https://i.imgur.com/KM6oCIn.png)

## CS50
This was my final project for the **CS50: Introduction to Computer Sciense**, a course offered by Havard University and taught by professor David Malan. The goal of the final project was to apply the knowledge acquired throughout the course from C programming, memory management, algorithms and data structures. The result is a **graphical game written in C**, using GTK4 for the interface and SDL2 for the sound effects.


## Overview
The game allows two players to play locally on the same computer, the interface consists of a basic 3x3 grid reprenting the board, where each player click on a cell to place their symbol (X or O), and the game checks for a win or draw. After a game ends, the board reset for a new game immediately.

Each move triggers a sound effect played via SDL2, sounds are implemented for:

- Valid moves.
- Winning and Draw moves.

This increases the user experience and makes the game more engaging.

## Project Layout 
The project is organized into one single C source file, and others folders for the style, images and sounds of the game.

    tictactoe/
    ├── assets/
    │ ├── css/      # CSS Style
    │ ├── icon.png  # Application icon
    │ ├── images/   # Game images
    │ └── sounds/   # Audio files
    ├── main.c      # Source code
    ├── Makefile    # Build file
    ├── install.sh  # Installation script
    └── README.md   # Project documentation

## Game Logic
To implement **ultra-efficient win detection**, I used a **bitboard approach** inspired by advanced Stack Overflow discussions on manipulating bits for game logic. Instead of storing the board as a 2D array, each player’s moves are represented as a **32-bit integer** where each bit encodes participation in rows, columns, and diagonals.

Each square has a 32-bit value, representing winlines it belongs to:

```c 
  const int32_t positions[9] = {
   0x80080080, // Row A  Column 1
   0x40008000, // Row A  Column 2
   0x20000808, // Row A  Column 3
   0x08040000, // Row B  Column 1 
   0x04004044, // Row B  Column 2
   0x02000400, // Row B  Column 3
   0x00820002, // Row C  Column 1
   0x00402000, // Row C  Column 2
   0x00200220  // Row C  Column 3
  };
```

When a player makes a move, their board is update with a simple **bitwise OR** operation and the currently player:

```c 
    game->players.PlayerO |= game->positions[index].position;
    game->positions[index].player = PLAYER_O;    
  
      
    game->players.PlayerX |= game->positions[index].position;
    game->positions[index].player = PLAYER_X;
```

Win detection uses a set of **bit masks**, represents all possible winning combinations in the game (3 rows, 3 columns, 2 diagonals). Each element in the array is a win mask created by combinning the bits of the board positions that form a row, column or a diagonal:

```c 
  const int32_t WINS[8] = {
    0xE0088888,
    0x0E044444,
    0x00E22222,
    0x888E0082,
    0x4440E044,
    0x22200E28,
    0x842842E4,
    0x2482484E
  };
```

A player wins if any of the **bits masks** is satisfied:

```c 
  for(int i = 0; i < 8; i++) {
    if((game->players.PlayerO & WINS[i]) == WINS[i]) {

      game->players.wins_o++;

      return PLAYER_O;
    }
  }

  for(int i = 0; i < 8; i++) {
    if((game->players.PlayerX & WINS[i]) == WINS[i]) {
      game->players.wins_x++;
      return PLAYER_X;
    }
  }
```

This allows the logic of the game run in a few bitwise operations, without iterating over rows, columns, or diagonals. Move validation is equally simple: a move is valid if thetarget square is not already set in a player.

This method is extremely fast, memory-efficient and elegant, demonstrating bit manipulation.

## Graphical Interface (GTK) 

The GUI of the game uses GTK4, for manipulationand handles input and signals of the widgets. Demonstrating a good understaning of pointers, memory management and even CSS for the style of the game.

## Sound Effect (SDL2) 

SDL2 is used for sound effects, providing a sound for users move or winning.

## Installation 

```bash 
cd tictactoe
chmod +x install.sh
./install.sh
./tictactoe
```

## Dependencies
All dependencies are automatically installed by the install script (install.sh).
- **GTK 4** (`libgtk-4-dev`) 
- **SDl2** (`libsdl2-dev`)
- **Build tools**: `gcc`, `make`, `pkg-config`

## Demonstration on youtube
For the CS50 final project you have to make a video showning your project,
[My Final Project presentation](https://www.youtube.com/watch?v=xzVdJ_ptiTQ)

## Documentation
https://stackoverflow.com/questions/1056316/algorithm-for-determining-tic-tac-toe-game-over

https://lazyfoo.net/tutorials/SDL/21_sound_effects_and_music/index.php

https://docs.gtk.org/gtk4/index.html

## About CS50
CS50 is a openware course from Havard University and taught by David J. Malan

Introduction to the intellectual enterprises of computer science and the art of programming. This course teaches students how to think algorithmically and solve problems efficiently. Topics include abstraction, algorithms, data structures, encapsulation, resource management, security, and software engineering. Languages include C, Python, and SQL plus students’ choice of: HTML, CSS, and JavaScript (for web development).

Thank you for all CS50.

- Where I get CS50 course?
https://cs50.harvard.edu/x/2025/

[LinkedIn](https://www.linkedin.com/in/danielventurv/)
