#ifndef player_h
#define player_h
#include <ncurses.h>

typedef struct {
    int x;
    int y;
    char body;
} Player;


void draw_player(WINDOW *win, Player *p);

void refresh_pos(WINDOW *win, Player *p);

void handle_input(int input, Player *p, char **map);

#endif