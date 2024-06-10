#include <stdlib.h>
#include <ncurses.h>
#include "world.h"
#include "player.h"


#define WIDTH 250
#define HEIGHT 250


int main(void) {
    // if(!has_colors()) {
    //     printf("Your terminal doesnt support colors");
    //     return 0;
    // }
    char *map[] = {
        // "####################################################################################################",
        "####################",
        "#        ###########",
        "####### ############",
        "#######         ####",
        "####### #####      #",
        "####### ######  n  #",
        "###        ####    #",
        "##   ;;;;   ########",
        "##   ;;;;;   #######",
        "###        #########",
        "#####     ##########",
        "####################",
    };
    int map_rows = sizeof(map) / sizeof(map[0]);
    int map_cols = strlen(map[0]);

    Tile **parsed_map = parse_world(map, map_cols, map_rows);


    char player_body = '@';
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    Player p = { .x = 1, .y = 1, .body = player_body };
    WINDOW *win = newwin(map_rows, map_cols, p.x, p.y);
    
    const int dialogue_h =  5;
    const int dialogue_w = 50;
    WINDOW *dialogue = newwin(dialogue_h, dialogue_w, map_cols - dialogue_h, 0);
    box(dialogue, 0, 0);
    mvwprintw(dialogue, 3, 3, "WASD - MOVE | Q - QUIT");
    wrefresh(dialogue);

    GameManager gm = {
        .current_map = parsed_map, 
        .c_map_cols = map_cols, 
        .c_map_rows = map_rows, 
        .main_w = win,
        .dialog = dialogue,
        };

    draw_player(gm.main_w, &p);

    //gameloop
    while(1) {

        int input = wgetch(gm.main_w);
        refresh_pos(gm.main_w, &p);

        handle_input(input, &p, gm.current_map); 

        Events event = check_for_event(p.x, p.y, gm);

        handle_event(event, gm);

        if(input == 'q') {
            break;
        } 

        // TODO: slow?
        draw_world(gm);
        draw_player(win, &p);

    }

    endwin();
    printf("The World is %dx%d\n", map_cols, map_rows);

}