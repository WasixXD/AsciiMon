#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

#include "world.h"
#include "battle.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void sleep_seconds(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}


const Move all_possible_moves[] = {
    {.name = "Tackle", .power = 40, .type = "Normal", .mp = 10 }, // 0
    {.name = "Ember", .power = 55, .type = "Fire", .mp = 8 }, // 1
    {.name = "Bubbles", .power = 55, .type = "Water", .mp = 8 }, // 2
    {.name = "Leafs", .power = 55, .type = "Grass", .mp = 8 }, // 3
    {.name = "Ice Spikes", .power = 55, .type = "Ice", .mp = 8 }, // 4
    {.name = "Bite", .power = 50, .type = "Dark", .mp = 12 }, // 5
    {.name = "Punch", .power = 50, .type = "Fighter", .mp = 12 }, // 6
    {.name = "Slash", .power = 45, .type = "Normal", .mp = 9 }, // 7
    {.name = "Flame Wheel", .power = 60, .type = "Fire", .mp = 6 }, // 8
    {.name = "Aqua Jet", .power = 53, .type = "Water", .mp = 10 }, // 9
    {.name = "Vine Whip", .power = 48, .type = "Grass", .mp = 11 }, // 10
    {.name = "Frost Bite", .power = 58, .type = "Ice", .mp = 7 }, // 11
    {.name = "Shadow Sneak", .power = 45, .type = "Dark", .mp = 11 }, // 12
    {.name = "Karate Chop", .power = 55, .type = "Fighter", .mp = 8 }, // 13
    {.name = "Headbutt", .power = 48, .type = "Normal", .mp = 11 }, // 14
    {.name = "Fire Spin", .power = 65, .type = "Fire", .mp = 5 }, // 15
    {.name = "Water Pulse", .power = 58, .type = "Water", .mp = 7 }, // 16
    {.name = "Solar Beam", .power = 100, .type = "Grass", .mp = 4 }, // 17
    {.name = "Ice Beam", .power = 63, .type = "Ice", .mp = 6 }, // 18
    {.name = "Dark Pulse", .power = 55, .type = "Dark", .mp = 8 }, // 19
    {.name = "Dynamic Punch", .power = 60, .type = "Fighter", .mp = 5 }, // 20
    {.name = "Body Slam", .power = 45, .type = "Normal", .mp = 11 }, // 21
    {.name = "Blaze Kick", .power = 58, .type = "Fire", .mp = 7 }, // 22
    {.name = "Hydro Pump", .power = 65, .type = "Water", .mp = 5 }, // 23
    {.name = "Leaf Blade", .power = 53, .type = "Grass", .mp = 9 }, // 24
    {.name = "Blizzard", .power = 70, .type = "Ice", .mp = 4 }, // 25
    {.name = "Night Slash", .power = 50, .type = "Dark", .mp = 8 }, // 26
    {.name = "Mach Punch", .power = 48, .type = "Fighter", .mp = 10 }, // 27
    {.name = "Hyper Beam", .power = 75, .type = "Normal", .mp = 3 }, // 28
    {.name = "Inferno", .power = 68, .type = "Fire", .mp = 5 }, // 29
    {.name = "Surf", .power = 60, .type = "Water", .mp = 6 }, // 30
    {.name = "Giga Drain", .power = 58, .type = "Grass", .mp = 7 }, // 31
    {.name = "Hailstorm", .power = 65, .type = "Ice", .mp = 5 }, // 32
    {.name = "Shadow Claw", .power = 53, .type = "Dark", .mp = 9 }, // 33
    {.name = "Focus Punch", .power = 68, .type = "Fighter", .mp = 5 }, // 34
    {.name = "Quick Attack", .power = 35, .type = "Normal", .mp = 13 }, // 35
    {.name = "Flamethrower", .power = 63, .type = "Fire", .mp = 6 }, // 36
    {.name = "Bubble Beam", .power = 55, .type = "Water", .mp = 8 }, // 37
    {.name = "Razor Leaf", .power = 48, .type = "Grass", .mp = 11 }, // 38
    {.name = "Ice Fang", .power = 53, .type = "Ice", .mp = 9 }, // 39
};

char* int_to_string(int num) {
    
    int length = snprintf(NULL, 0, "%d", num);
    char *str = (char *)malloc(length + 1);
    if (str == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(1);
    }
    sprintf(str, "%d", num);
    return str;
}

void draw_world(GameManager gm) {
    for(int i = 0; i < gm.c_map_rows; i++) {
        for(int j = 0; j < gm.c_map_cols; j++) {
            wattron(gm.main_w, COLOR_PAIR(gm.current_map[i][j].pair));
            mvwaddch(gm.main_w, i, j, gm.current_map[i][j].sprite);
            wrefresh(gm.main_w);
            wattroff(gm.main_w, COLOR_PAIR(gm.current_map[i][j].pair));
        }
    }
}

void draw_dialogue(WINDOW *dialogue, int y, int x, char *diag) {
    mvwprintw(dialogue, y, x, diag);
    box(dialogue, 0, 0);
    wrefresh(dialogue);
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
                    Tile wall_tile = (Tile){.walkable = false, .type = "WALL", .x = j, .y = i, .sprite = '#', .pair = WALL};
                    parsed[i][j] = wall_tile;
                    break;

                }
                case ' ': {
                    Tile ground_tile = (Tile){.walkable = true, .type = "GROUND", .x = j, .y = i, .sprite = ' ', .pair = GROUND};
                    parsed[i][j] = ground_tile;
                    break;
                }
                case ';': {
                    Tile grass_tile = (Tile){.walkable = true, .type = "GRASS", .x = j, .y = i, .sprite = 'w', .pair = GRASS};
                    parsed[i][j] = grass_tile;
                    break;
                }
                case 'n': {
                    Tile npc_tile = (Tile){.walkable = false, .type = "NPC", .x = j, .y = i, .sprite = 'n', .pair = NPC};
                    parsed[i][j] = npc_tile;
                    break;
                }
                case 'T': {
                    Tile trainer_tile = (Tile){.walkable = false, .type = "TRAINER", .x = j, .y = i, .sprite = 'T', .pair = TRAINER};
                    parsed[i][j] = trainer_tile;
                    break;
                }
            }
        }
    }
    return parsed;
}


// In theory the game can only handle one event per time, so, its better to avoid having a battle aside a npc or a trainer
Events check_for_event(int p_x, int p_y, GameManager gm) {
    Tile current_player_tile = gm.current_map[p_y][p_x];
    srand(time(NULL));

    if(current_player_tile.type == "GRASS") {
        double chance_to_battle = (double)rand() / RAND_MAX;
        if(chance_to_battle >= 0.5) {
            return WILD_MON_BATTLE;
        }
    }

    // Check for npc
    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            Tile check_for_npc = gm.current_map[p_y + i][p_x + j];
            if(check_for_npc.type == "NPC") {
                return NPC_DIALOG;
            }
        }
    }


    //Check for trainer
    int directions[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
    for(int i = 0; i < 4; i++) {
        int x = p_x;
        int y = p_y;
        for(int dist = 0; dist < 3; dist++) {
            x += directions[i][0];
            y += directions[i][1];

            Tile trainer_tile = gm.current_map[y][x];

            if(x < 0 || x >= gm.c_map_rows || y < 0 || y >= gm.c_map_cols) break;

            if(trainer_tile.type == "WALL") break;

            if(trainer_tile.type == "TRAINER") {
                return TRAINER_BATTLE;
            }
        }
    }
    return NONE;
}


void handle_event(Events e, GameManager gm, Player *p) {
    switch(e) {
        case WILD_MON_BATTLE: {
            sleep_seconds(1);
            battle(p, gm);
            draw_world(gm);
            break;
        }

        case NPC_DIALOG: {
            wclear(gm.dialog);
            draw_dialogue(gm.dialog, 1, 3, "Hey, what's up?"); 
            break;
        }

        case NONE: {
            wclear(gm.dialog);
            break;
        }
        
        case TRAINER_BATTLE: {
            wclear(gm.dialog);
            draw_dialogue(gm.dialog, 1, 3, "Hey, let's battle");

            int t_n_mons = (rand() % 3) +  1;
            break;
        }
    }
}

int txt_filter(const struct dirent *entry) {
    const char *ext = strrchr(entry->d_name, '.');
    return (ext && strcmp(ext, ".txt") == 0);
}

void remove_extension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (dot && strcmp(dot, ".txt") == 0) {
        *dot = '\0';
    }
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
        remove_extension(namelist[n]->d_name);
        new_mon.name = namelist[n]->d_name;
        new_mon.lvl = 1;
        new_mon.xp_points = 1;
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

        new_mon.total_moves = mon_move_i;
        gm->all_mons[mon_i] = new_mon;
        fclose(fptr);
        free(namelist[n]);
        mon_i++;

    }
    gm->q_mons = mon_i;
    free(namelist);
}

