# Game of Life

Implementation of Conway's Game of Life - <https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life>

### Build Dependencies

```
libncurses5
```

### Makefile

- make
- make clean

## Usage

```
Usage: game_of_life [-l] [-s] [-h]
    -l
        The number of randomly populated live cells on startup. (If not supplied default parameter is used)
    -s
        The number of seconds between each generation. (If not supplied default parameter is used)
    -h
        Print this help message
```

## Built & tested with

- Debian GNU/Linux 9 (stretch)
- gcc/g++ 6.3.0
