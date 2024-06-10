#include "world.h"
#include <stdbool.h>
#include <ncurses.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>



// #include "player.h"

void draw_world(GameManager gm) {

    for(int i = 0; i < gm.c_map_rows; i++) {
        for(int j = 0; j < gm.c_map_cols; j++) {
            mvwaddch(gm.main_w, i, j, gm.current_map[i][j].sprite);
            wrefresh(gm.main_w);
        }
    }
}

bool is_walkable(Tile **map, int next_x, int next_y) {
    return map[next_y][next_x].walkable;
}

Tile** parse_world(char **map, int map_cols, int map_rows) {

    Tile **parsed = malloc(map_rows * sizeof(Tile *));
    for(int i = 0; i < map_rows; i++) {
        parsed[i] = malloc(map_cols * sizeof(Tile));
    }

    for(int i = 0; i < map_rows; i++) {
        for(int j = 0; j < map_cols; j++) {
            char current = map[i][j];

            switch(current) {
                case '#': {
                    Tile wall_tile = (Tile){.walkable = false, .type = "WALL", .x = j, .y = i, .sprite = '#'};
                    parsed[i][j] = wall_tile;
                    break;

                }
                case ' ': {
                    Tile ground_tile = (Tile){.walkable = true, .type = "GROUND", .x = j, .y = i, .sprite = '.'};
                    parsed[i][j] = ground_tile;
                    break;
                }
                case ';': {
                    Tile grass_tile = (Tile){.walkable = true, .type = "GRASS", .x = j, .y = i, .sprite = ';'};
                    parsed[i][j] = grass_tile;
                    break;
                }
                case 'n': {
                    Tile npc_tile = (Tile){.walkable = false, .type = "NPC", .x = j, .y = i, .sprite = 'n'};
                    parsed[i][j] = npc_tile;
                    break;
                }
            }
        }
    }
    return parsed;
}

Events check_for_event(int p_x, int p_y, GameManager gm) {
    Tile current_player_tile = gm.current_map[p_y][p_x];
    srand(time(NULL));

    if(current_player_tile.type == "GRASS") {
        double chance_to_battle = (double)rand() / RAND_MAX;
        if(chance_to_battle >= 0.5) {
            return WILD_MON_BATTLE;
        }
    }

    // TODO: Refactor 
    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            Tile check_for_npc = gm.current_map[p_y + i][p_x + j];
            if(check_for_npc.type == "NPC") {
                return NPC_DIALOG;
            }
        }
    }
    return NONE;
}


void handle_event(Events e, GameManager gm) {
    switch(e) {
        case WILD_MON_BATTLE: {
            // TODO: Refactor
            WINDOW *battle_win = newwin(gm.c_map_rows + 1, gm.c_map_cols + 1, 1, 1);
            mvwprintw(battle_win, 1, 1,"Time to Battle");
            mvwprintw(battle_win, 2, 1, "Try do run");

            wrefresh(battle_win);
            int input;
            while(1) {
                input = wgetch(battle_win);
                if(input == 'f') {
                    break;
                }
            }
            
            draw_world(gm);
            break;
        }

        case NPC_DIALOG: {
            mvwprintw(gm.dialog, 1, 1, "A sport game, Dad'll like that!");
            wrefresh(gm.dialog); 
            break;
        }
    }
}