#include <stdlib.h>
#include <ncurses.h>
#include "player.h"
#include "world.h"


#define WIDTH 250
#define HEIGHT 250



int main(void) {
    // if(!has_colors()) {
    //     printf("Your terminal doesnt support colors");
    //     return 0;
    // }
    char *map[] = {
        "####################",
        "#        ###########",
        "####### ############",
        "####### ############",
        "####### ############",
        "####### ############",
        "######   ###########",
        "####      ##########",
        "####         #######",
        "####################",
    };
    int map_rows = sizeof(map) / sizeof(map[0]);
    int map_cols = strlen(map[0]);

    printf("%d\n%d", map_rows, map_cols);

    char player_body = '@';
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    Player p = { .x = 1, .y = 1, .body = player_body };
    WINDOW *win = newwin(map_rows, map_cols, p.x, p.y);

    
    

    draw_world(win, map, map_cols, map_rows);
    draw_player(win, &p);
    //gameloop
    while(1) {

        int input = wgetch(win);
        refresh_pos(win, &p);

        handle_input(input, &p, map); 

        if(input == 'q') {
            break;
        } 

        draw_player(win, &p);

    }

    endwin();

}