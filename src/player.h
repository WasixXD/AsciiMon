#ifndef player_h
#define player_h
#include <ncurses.h>
#include "world.h"

void draw_player(WINDOW *win, Player *p);

void refresh_pos(WINDOW *win, Player *p);

void handle_input(int input, Player *p, Tile **map);

bool add_mon(Player *p, Mon *new_mon);

#endif