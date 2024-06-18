#include <ncurses.h>
#include "world.h"
#include "player.h"
#include <string.h>

int main(void) {
    // if(!has_colors()) {
    //     printf("Your terminal doesnt support colors");
    //     return 0;
    // }
    char *map[] = {
        "###########################################################################",
        "#          ################################################################",
        "#          ################################################################",
        "#          ################################################################",
        "#                   #######################################################",
        "#####  n     ;;;;;   ######################################################",
        "#####        ;;;;;   ######################################################",
        "########     ;;;;;   ######################################################",
        "########            #######################################################",
        "###########T###############################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        "###########################################################################",
        
    };
    int map_rows = sizeof(map) / sizeof(map[0]);
    int map_cols = strlen(map[0]);

    Tile **parsed_map = parse_world(map, map_cols, map_rows);


    char player_body = '@';
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    const int dialogue_h =  5;
    const int dialogue_w = 50;
    WINDOW *dialogue = newwin(dialogue_h, dialogue_w, map_rows + 2, 0);
    

    start_color();
    init_pair(GROUND, COLOR_BLACK, COLOR_BLACK);
    init_pair(GRASS, COLOR_WHITE, COLOR_GREEN);
    init_pair(WALL, COLOR_WHITE, COLOR_BLACK);
    init_pair(NPC, COLOR_YELLOW, COLOR_BLACK);
    init_pair(TRAINER, COLOR_RED, COLOR_BLACK);

    Player p = { .x = 1, .y = 1, .body = player_body };

    WINDOW *win = newwin(map_rows, map_cols, p.x, p.y);

    GameManager gm = {
        .current_map = parsed_map, 
        .c_map_cols = map_cols, 
        .c_map_rows = map_rows, 
        .main_w = win,
        .dialog = dialogue,
    };
    allocate_mons(&gm);
    p.mons[0] = gm.all_mons[1]; 

    p.n_of_mons += 1;

    p.items[0] = (Item){ .name = "Potion", .desc = "Restore 10 HP \n from your mon", .quantity = 10 };
    p.items[1] = (Item){ .name = "MonBall",.desc = "Used to capture \n Mons", .quantity = 10};

    draw_dialogue(gm.dialog, 3, 1, "wasd - MOVE | q - QUIT | e - ITENS | f - MONS");
    draw_world(gm);
    draw_player(gm.main_w, &p);

    //gameloop
    while(1) {

        int input = wgetch(gm.main_w);
        refresh_pos(gm.main_w, &p);

        handle_input(input, &p, gm.current_map); 

        Events event = check_for_event(p.x, p.y, gm);

        handle_event(event, gm, &p);

        if(input == 'q') {
            break;
        } 

        if(input == 'e') {
           

            draw_dialogue(gm.dialog, 1, 1, "w - MOVE UP | s - MOVE DOWN | e - EXIT");

            int _ = get_some_item(&p, p.x);

            wclear(gm.dialog);
       }

        if(input == 'f') {
              
            draw_dialogue(gm.dialog, 1, 1, "w - MOVE UP | s - MOVE DOWN | f - EXIT |\n space - CHANGE");
            int _ = choose_mon(&p, p.x);

            wclear(gm.dialog);
        }
        draw_world(gm);
        draw_dialogue(gm.dialog, 3, 1,  "wasd - MOVE | q - QUIT | e - ITENS | f - MONS");
        draw_player(gm.main_w, &p);

    }

    endwin();
    printf("The World is %dx%d\n", map_cols, map_rows);

}