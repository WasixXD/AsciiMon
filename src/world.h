#ifndef world_h
#define world_h

// #include "player.h"
#include <ncurses.h>
#include <stdbool.h>

#define MAX_MOVES 4
#define MAX_MONS 6

#define GROUND 0
#define WALL 1
#define GRASS 2
#define NPC 3
#define TRAINER 4
#define STAR 5


typedef struct {
    char *name;
    char *type;
    int power;
    int mp;
    int move_mp;
} Move;

typedef struct {
    int max_hp;
    int current_hp;
    int lvl;
    int xp_points;
    int total_moves;
    char *name;
    char *type;
    char sprite[3][8];
    Move moves[MAX_MOVES];
} Mon;

typedef struct {
    char *name;
    char *desc;
    int quantity;
} Item;

typedef struct {
    int x;
    int y;
    char body;
    Mon mons[MAX_MONS];
    int n_of_mons;
    Item items[2];
} Player;

typedef struct {
    char *name;
    Mon mons[MAX_MONS];
    int n_mons;
} Trainer;

typedef enum  {
    WILD_MON_BATTLE,
    NPC_DIALOG,
    TRAINER_BATTLE,
    NONE,
    GAME_FINISH,
    HEAL_MONS,
} Events ;


typedef struct {
    bool walkable;
    char *type;
    char sprite;
    int x;
    int y;
    int pair;
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

void draw_items(Player *p);

bool is_walkable(Tile **map, int nextX, int nextY);

Tile** parse_world(char **map, int map_cols, int map_rows);

Events check_for_event(int p_x, int p_y, GameManager gm);

void allocate_mons(GameManager *gm);

void handle_event(Events e, GameManager gm, Player *p);

char* int_to_string(int num);

void sleep_seconds(int seconds);

int get_some_item(Player *p, int x_coordinate);

int choose_mon(Player *p, int x_coordinate);

void heal_mons(Mon mons[], int n_mons, int n_moves);

#endif