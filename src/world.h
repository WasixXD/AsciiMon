#ifndef world_h
#define world_h

// #include "player.h"
#include <ncurses.h>
#include <stdbool.h>

#define MAX_MOVES 4
#define MAX_MONS 6


void sleep_seconds(int seconds);

typedef struct {
    char *name;
    char *type;
    int power;
    int mp;

} Move;

typedef struct {
    int max_hp;
    int current_hp;
    int lvl;
    int total_moves;
    char *name;
    char *type;
    char sprite[3][8];
    Move moves[MAX_MOVES];
} Mon;

typedef struct {
    int x;
    int y;
    char body;
    Mon mons[MAX_MONS];
} Player;

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
    Mon *all_mons;
    int q_mons;
} GameManager;

void draw_world(GameManager gm);

void draw_dialogue(WINDOW *dialogue, int y, int x, char *diag);

bool is_walkable(Tile **map, int nextX, int nextY);

Tile** parse_world(char **map, int map_cols, int map_rows);

Events check_for_event(int p_x, int p_y, GameManager gm);

void allocate_mons(GameManager *gm);

void handle_event(Events e, GameManager gm, Player *p);

#endif