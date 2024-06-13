#include "battle.h"
#include "world.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

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

    bar[0] = '[';
    for(int i = 1; i < bar_length - 1; i++) {
        bar[i] = i < icons? '#' : ' ';
    }
    bar[bar_length - 1] = ']';

    bar[bar_length] = '\0';

    return bar;
}

void draw_status(WINDOW *win, Mon *mon, int y, int x) {
    mvwaddstr(win, y, x - (strlen(mon->name) + 2), mon->name);
    mvwaddstr(win, y, x - (strlen(mon->name) + 4), int_to_string(mon->lvl));   
    mvwaddstr(win, y + 1, x - (strlen(mon->name) + 4), health_bar(mon->max_hp, mon->current_hp));
}


void draw_options(WINDOW *win, int y, int x, char *text) {
    box(win, 0, 0);
    mvwaddstr(win, y, x, text);
    wrefresh(win);

}

void clear_input_buffer(WINDOW *win) {
    nodelay(win, TRUE);
    while (wgetch(win) != ERR) {}
    nodelay(win, FALSE);
}

int calc_dmg(int nivel, int poder, int mod) {
    float rand_factor = (rand() % 16 + 85) / 100.0;
    
    int dmg = (nivel * poder * mod / 10) * rand_factor;
    return dmg;
}


void apply_dmg(Mon *mon, int dmg) {
    if(mon->current_hp > 0) {
        mon->current_hp -= dmg;

    }
}


bool check_if_defeated(Mon *mon) {
    return mon->current_hp <= 0;
}

void battle(Player *p, GameManager gm) {
    wclear(gm.main_w);
    wclear(gm.dialog);
    wrefresh(gm.main_w);
    wrefresh(gm.dialog);

    int turn = 0;

    srand(time(NULL));
    int battle_w_w = 60;
    int battle_w_h = 20;
    WINDOW *battle_win = newwin(battle_w_h, battle_w_w, 0, 0);
    box(battle_win, 0, 0);  

    WINDOW *options = newwin(3, battle_w_w, battle_w_h, 0);
 
    int random_mon_index = rand() % gm.q_mons;
    Mon random_mon = gm.all_mons[random_mon_index];  

    int sprite_padding_w = 10;
    int sprite_padding_h =  5;

    int r_mon_x = battle_w_w - sprite_padding_w;
    int r_mon_y = 2;

    int r_mon_name_len = strlen(random_mon.name);

    int p_mon_y = battle_w_h - sprite_padding_h;
    int p_mon_x = 2;

    int input;
    wclear(options);
    draw_options(options, 1, 1, random_mon.name);
    draw_options(options, 1, r_mon_name_len + 1, " Attacks you!");

    while(1) {
        draw_mon(battle_win, &p->mons[0], p_mon_y, p_mon_x);
        draw_status(battle_win, &p->mons[0], p_mon_y, p_mon_y + 10);

        draw_mon(battle_win, &random_mon, r_mon_y, r_mon_x);
        draw_status(battle_win, &random_mon, r_mon_y, r_mon_x);

        if(check_if_defeated(&p->mons[0])) {
            // TODO: Instead of losing, let the player have a chance to change mon
            wclear(options);
            draw_options(options, 1, 1, "Your mon was defeated");

            sleep_seconds(2);
            break;
        }


        // Player turn
        if(turn == 0) {
            sleep_seconds(2);
            wclear(options);
            draw_options(options, 1, 1, "q - Attack | w - Mons | e - Itens | r - Run");

            do {
                input = wgetch(battle_win);
            } while (input != 'q' && input != 'w' && input != 'e' && input != 'r');

            if(input == 'r') {
                break;
            }
            if(input == 'q') {
                wclear(options);
                apply_dmg(&random_mon, 10); 
                draw_options(options, 1, 1, "Você atacou!");

                sleep_seconds(1);
            }
            turn = 1; 

            if(check_if_defeated(&random_mon)) {
                wclear(options);
                draw_options(options, 1, 1, random_mon.name);
                draw_options(options, 1, r_mon_name_len + 1, " was defeated");
                sleep_seconds(2);
                break;
            }

        } else {   
            // Mon turn
            sleep_seconds(2);
            wclear(options);
            
            int random_move_index = rand() % random_mon.total_moves;
            Move random_move = random_mon.moves[random_move_index];

            if(random_move.mp > 0) {
                draw_options(options, 1, 1, random_mon.name);
                draw_options(options, 1, r_mon_name_len + 1, " Usou ");
                draw_options(options, 1, r_mon_name_len + 7, random_move.name);

                // TODO: Find a way to calculate how effective was the dmg
                int mod = 1;
                int dano = calc_dmg(random_mon.lvl, random_move.power, mod);

                apply_dmg(&p->mons[0], dano);
                sleep_seconds(1);
            }

            clear_input_buffer(battle_win);
            turn = 0;
        }
        
    }

    wclear(options);
    wrefresh(options);

    wclear(battle_win);
    wrefresh(battle_win);
}
