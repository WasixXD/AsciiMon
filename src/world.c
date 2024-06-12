#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

#include "world.h"
#include "player.h"
#include "battle.h"

// lazy
const Move all_possible_moves[] = {
    {.name = "Tackle", .power = 80, .type = "Normal", .mp = 20 },
    {.name = "Ember", .power = 110, .type = "Fire", .mp = 15 },
    {.name = "Bubbles", .power = 110, .type = "Water", .mp = 15 },
    {.name = "Leafs", .power = 110, .type = "Grass", .mp = 15 },
    {.name = "Ice Spikes", .power = 110, .type = "Ice", .mp = 15 },
    {.name = "Bite", .power = 100, .type = "Dark", .mp = 25 },
    {.name = "Punch", .power = 100, .type = "Fighter", .mp = 25 },
};

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


// In theory the game can only handle one event per time, so, its better to avoid having a battle aside a npc
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


void handle_event(Events e, GameManager gm, Player *p) {
    switch(e) {
        case WILD_MON_BATTLE: {
            battle(p);
            draw_world(gm);
            break;
        }

        case NPC_DIALOG: {
            mvwprintw(gm.dialog, 1, 1, "Testando...ç?#");
            wrefresh(gm.dialog); 
            break;
        }

        case NONE: {
            wrefresh(gm.dialog);
            break;
        }
    }
}

int txt_filter(const struct dirent *entry) {
    const char *ext = strrchr(entry->d_name, '.');
    return (ext && strcmp(ext, ".txt") == 0);
}


// This had added a high level of abstraction
void allocate_mons(GameManager *gm) {
    const char *mon_folder_path = "./mons/";
    struct dirent **namelist;
    int n; 
    int mon_i = 0;
    n = scandir(mon_folder_path, &namelist, txt_filter, alphasort);

    gm->all_mons = (Mon*)malloc(n * sizeof(Mon));
    if(n < 0) {
        perror("Scandir");
        return;
    }


    while(n--) {
        int parser_p = 0; 
        int mon_move_i = 0;
        int sprite_row = 0;
        Mon new_mon;

        char filePath[50];
        // concat ./mons/ + name of the file
        snprintf(filePath, sizeof(filePath), "%s%s", mon_folder_path, namelist[n]->d_name);
        new_mon.name = namelist[n]->d_name;
        new_mon.lvl = 1;
        // open file
        FILE *fptr = fopen(filePath, "r");   
        if(fptr != NULL) {
            int bufferL = 255;
            char buffer[bufferL];

            //read each line
            while(fgets(buffer, bufferL, fptr) != NULL) {
                buffer[strcspn(buffer, "\n")] = 0;

                if(strcmp(buffer, "___") == 0) {
                    parser_p++;
                } else {
                    // if 0, then we reading the sprites
                    if(parser_p == 0) {
                        strcpy(new_mon.sprite[sprite_row], buffer);
                        sprite_row++;
                    } else if(parser_p == 1) { // if 1, then max_hp
                        new_mon.max_hp = atoi(buffer);
                        new_mon.current_hp = atoi(buffer);
                    } else if(parser_p == 2) { //if 2, then type
                        new_mon.type = strdup(buffer);
                    } else if(parser_p == 3) { //if 3, then we reading moves
                        int move_index = atoi(buffer);
                        Move c_move = all_possible_moves[move_index];
                        new_mon.moves[mon_move_i] = c_move;
                        mon_move_i++;
                    }

                }
                
            } 

            
        } else {
            printf("É NULL");
        }

        gm->all_mons[mon_i] = new_mon;
        fclose(fptr);
        free(namelist[n]);
        mon_i++;

    }

    
    free(namelist);
}