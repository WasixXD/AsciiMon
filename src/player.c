#include "player.h"
#include <ncurses.h>
#include "world.h"

void refresh_pos(WINDOW *win, Player *p) {
    mvwaddch(win, p->y, p->x, ' ');
    wrefresh(win);
    
}

void handle_input(int input, Player *p, Tile **map) {
    if(input == 'w' && is_walkable(map, p->x, p->y - 1)) {
        p->y--;
    } else if(input == 's' && is_walkable(map, p->x, p->y + 1)) {
        p->y++;
    } else if(input == 'd' && is_walkable(map, p->x + 1, p->y)) {
        p->x++;
    } else if(input == 'a' && is_walkable(map, p->x - 1, p->y)) {
        p->x--;
    }
}


void draw_player(WINDOW *win, Player *p) {
    mvwaddch(win, p->y, p->x, p->body);
    wrefresh(win);   
}