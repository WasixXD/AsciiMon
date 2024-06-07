#ifndef world_h
#define world_h

#include <ncurses.h>
#include <stdbool.h>
#include "player.h"

void draw_world(WINDOW *win, char **map, int map_cols, int map_rows);


bool is_walkable(char **map, int nextX, int nextY);

#endif