#ifndef world_h
#define world_h

// #include "player.h"
#include <ncurses.h>
#include <stdbool.h>

typedef enum  {
    WILD_MON_BATTLE,
    NPC_DIALOG,
    TRAINER_BATTLE,
    NONE,
} Events ;


typedef struct {
    bool walkable;
    char *type;
    char sprite;
    int x;
    int y;
} Tile;


typedef struct {
    WINDOW *main_w;
    Tile **current_map;
    int c_map_rows;
    int c_map_cols;
    WINDOW *dialog;
} GameManager;

void draw_world(GameManager gm);

bool is_walkable(Tile **map, int nextX, int nextY);

Tile** parse_world(char **map, int map_cols, int map_rows);


Events check_for_event(int p_x, int p_y, GameManager gm);

void handle_event(Events e, GameManager gm);

#endif