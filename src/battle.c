#include "battle.h"
#include "world.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>


void draw_mon(WINDOW *win, Mon *mon, int x, int y) {
    for(int i = 0; i < 3; i++) {
        mvwaddstr(win, x + i,  y, mon->sprite[i]); 
        wrefresh(win);
    }

}

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


char* health_bar(int max_health, int current_health) {
    int bar_length = 12;
    int icons = (current_health * bar_length) / max_health;
    char *bar = (char *)malloc(bar_length);

    for(int i = 0; i < bar_length; i++) {
        bar[i] = i < icons? '#' : ' ';
    }

    bar[bar_length] = '\0';

    return bar;
}

void battle(Player *p, GameManager gm) {
    wclear(gm.main_w);
    wclear(gm.dialog);
    wrefresh(gm.main_w);
    wrefresh(gm.dialog);

    srand(time(NULL));
    int battle_w_w = 60;
    int battle_w_h = 20;
    WINDOW *battle_win = newwin(battle_w_h, battle_w_w, 0, 0);
    box(battle_win, 0, 0);  
    
    int random_mon_index = rand() % gm.q_mons;
    Mon random_mon = gm.all_mons[random_mon_index];  
    
    // WINDOW *r_mon_win = newwin(10, 10, 2, 50);
    // box(r_mon_win, 0 ,)

    int sprite_padding_w = 10;
    int sprite_padding_h =  5;

    int r_mon_x = battle_w_w - sprite_padding_w;
    int r_mon_y = 2;

    int input;

    while(1) {
        draw_mon(battle_win, &p->mons[0], battle_w_h - sprite_padding_h, 2);
        draw_mon(battle_win, &random_mon, r_mon_y, r_mon_x);

        // TODO: Refactor
        mvwaddstr(battle_win, r_mon_y, r_mon_x - (strlen(random_mon.name) + 2), random_mon.name);
        mvwaddstr(battle_win, r_mon_y, r_mon_x - (strlen(random_mon.name) + 4), int_to_string(random_mon.lvl));
        
        mvwaddstr(battle_win, r_mon_y + 1, r_mon_x - (strlen(random_mon.name) + 4), health_bar(random_mon.max_hp, random_mon.current_hp / 2));


        input = wgetch(battle_win);
        if(input == 'f') {
            wclear(battle_win);
            wrefresh(battle_win);
            break;
        }
    }
}


