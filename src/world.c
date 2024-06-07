#include "world.h"
#include <stdbool.h>
#include <ncurses.h>
#define WALL_COLOR 1


void draw_world(WINDOW *win, char **map, int map_cols, int map_rows) {

    start_color();
    init_pair(WALL_COLOR, COLOR_YELLOW, COLOR_BLUE);
    for(int i = 0; i < map_rows; i++) {
        for(int j = 0; j < map_cols; j++) {
            wattron(win, COLOR_PAIR(WALL_COLOR));
            mvwaddch(win, i, j, map[i][j]);
            wattroff(win, COLOR_PAIR(WALL_COLOR));
            wrefresh(win);
        }
    }
}


bool is_walkable(char **map, int next_x, int next_y) {
    // TODO: Refactor
    
    return map[next_y][next_x] == ' ';
}