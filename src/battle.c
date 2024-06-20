#include "battle.h"
#include "world.h"
#include "player.h"
#include <ncurses.h>
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
    mvwaddstr(win, y, x, int_to_string(mon->lvl));   
    mvwaddstr(win, y, x + 2 + strlen(int_to_string(mon->lvl)), mon->name);
    mvwaddstr(win, y + 1, x, health_bar(mon->max_hp, mon->current_hp));
    wrefresh(win);
}


void clear_input_buffer(WINDOW *win) {
    nodelay(win, TRUE);
    while (wgetch(win) != ERR) {}
    nodelay(win, FALSE);
}

int calc_dmg(int nivel, int poder, int mod) {
    float rand_factor = (rand() % 16 + 85) / 100.0;
    
    int dmg = (((nivel * poder) / 10) / (rand_factor * nivel)) * mod;
    return dmg;
}

void apply_dmg(Mon *mon, int dmg) {
    if(mon->current_hp > 0) {
        mon->current_hp -= dmg;
        mon->current_hp = mon->current_hp < 0? 0 : mon->current_hp;
    }
}

bool check_if_defeated(Mon *mon) {

    return mon->current_hp <= 0;
}

int effectiveness(char *move_type, char *mon_type) {
    if(strcmp(move_type, "Psychic") == 0 && strcmp(mon_type, "Dark") == 0) {
        return 2;
    } else if(strcmp(move_type, "Fire") == 0 && strcmp(mon_type, "Grass") == 0) {
        return 2;
    } else if(strcmp(move_type, "Water") == 0 && strcmp(mon_type, "Fire") == 0) {
        return 2;
    } else if(strcmp(move_type, "Grass") == 0 && strcmp(mon_type, "Water") == 0) {
        return 2;
    } else if(strcmp(move_type, "Ice") == 0 && strcmp(mon_type, "Fighter") == 0) {
        return 2;
    } else if(strcmp(move_type, "Dark") == 0 && strcmp(mon_type, "Psychic") == 0) {
        return 2;
    } else if(strcmp(move_type, "Fighter") == 0 && strcmp(mon_type, "Ice") == 0) {
        return 2;
    }

    return 1;
}


bool lvl_up(Mon *mon) {
    //lvl up!
    if(mon->xp_points > mon->lvl) {
        int hp_lvl_factor = 7;
        mon->lvl++;
        mon->xp_points = 0;
        mon->max_hp += hp_lvl_factor;
        mon->current_hp += hp_lvl_factor;
        return true;
    }

    return false;
}

int get_mon_move(Mon *mon) {
    WINDOW *win_select = newwin(8, 20, 5, 8);
    WINDOW *win_options = newwin(8, 15, 5, 30);

    box(win_options, 0, 0);
    box(win_select, 0, 0);

    int current = 2;

    int input = 0;

    // first draw the moves 
    for(int i = 0; i < mon->total_moves; i++) {
        mvwaddstr(win_select, i + 2, 3, int_to_string(i + 1));
        mvwaddstr(win_select, i + 2, 5, mon->moves[i].name);
        wrefresh(win_select);
    }

    mvwaddch(win_select, current, 1, '*');

    do {
        input = wgetch(win_select);
        mvwaddch(win_select, current, 1, ' ');

        if(input == 'w' && current > 2) {
            current--;
        } else if(input == 's' && current <= mon->total_moves) {
            current++;
        }

        wclear(win_options);

        // handle what current is selected
        draw_dialogue(win_options, 2, 1, mon->moves[current - 2].name);
        draw_dialogue(win_options, 3, 1, int_to_string(mon->moves[current - 2].power));
        draw_dialogue(win_options, 4, 1, int_to_string(mon->moves[current - 2].mp));
        draw_dialogue(win_options, 5, 1, mon->moves[current - 2].type);

        mvwaddch(win_select, current, 1, '*');
        wrefresh(win_select);
    } while(input != 'f');

    wclear(win_options);
    wrefresh(win_options);
    delwin(win_options);

    wclear(win_select);
    wrefresh(win_select);
    delwin(win_select);

    // -2 because starts at 2
    return current - 2;
}

bool party_up(Mon mons[], int n_mons) {
    for (int i = 0; i < n_mons; i++) {
        if (mons[i].current_hp > 0) {
            return true;             
        }
    }
    return false;
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
    random_mon.lvl = rand() % p->mons[0].lvl + 1;
    random_mon.max_hp = random_mon.max_hp + (5 * random_mon.lvl);
    random_mon.current_hp = random_mon.max_hp;

    int sprite_padding_w = 10;
    int sprite_padding_h =  5;

    int r_mon_x = battle_w_w - sprite_padding_w;
    int r_mon_y = 2;

    int r_mon_name_len = strlen(random_mon.name);

    int p_mon_y = battle_w_h - sprite_padding_h;
    int p_mon_x = 2;

    int input;
    wclear(options);
    draw_dialogue(options, 1, 1, random_mon.name);
    draw_dialogue(options, 1, r_mon_name_len + 1, " is angry and attacks you!");


    WINDOW *mon_status = newwin(4, 14, p_mon_y - 1, p_mon_x + 10);

    WINDOW *r_mon_status = newwin(4, 14 , r_mon_y - 1, r_mon_x - 18);

    while(1) {
        draw_mon(battle_win, &p->mons[0], p_mon_y, p_mon_x);
        draw_status(mon_status, &p->mons[0], 1, 1);
        box(mon_status, 0, 0);  
      
        wrefresh(mon_status);

        draw_mon(battle_win, &random_mon, r_mon_y, r_mon_x);
        draw_status(r_mon_status, &random_mon, 1, 1);
        box(r_mon_status, 0, 0);
        wrefresh(r_mon_status);

        if(!party_up(p->mons, p->n_of_mons)) {
            wclear(options);
            draw_dialogue(options, 1, 1, "All your were defeated");
            sleep_seconds(1);
            break;
        } 

        if(check_if_defeated(&p->mons[0])) {
            wclear(options);
            draw_dialogue(options, 1, 1, "Your mon was defeated");
            sleep_seconds(1);

            choose_mon(p, battle_w_w / 2);
           
            wclear(options);
            draw_dialogue(options, 1, 1, "you changed mon");
            wclear(battle_win);
            box(battle_win, 0, 0);
            wrefresh(battle_win);
            sleep_seconds(1);
            continue;
        }


        // Player turn
        if(turn == 0) {
            sleep_seconds(2);
            wclear(options);
            draw_dialogue(options, 1, 1, "q - Attack | w - Mons | e - Itens | r - Run");

            do {
                input = wgetch(battle_win);
            } while (input != 'q' && input != 'w' && input != 'e' && input != 'r');

            if(input == 'r') {
                float chance_to_run = (float)rand() / RAND_MAX;
                if(chance_to_run >= 0.2) break;

                wclear(options);
                draw_dialogue(options, 1, 1, "You tried to run, but failed");
                
            }
            if(input == 'q') {
                wclear(options);
                draw_dialogue(options, 1, 1, "w - MOVE UP | s - MOVE DOWN | f - CHOOSE");
                int move_index = get_mon_move(&p->mons[0]);
                Move p_move = p->mons[0].moves[move_index];
                wclear(options);

                if(p_move.mp > 0) {
                    int mod = effectiveness(p_move.type, random_mon.type);
                    int dano = calc_dmg(p->mons[0].lvl, p_move.power, mod);
                    apply_dmg(&random_mon, dano); 
                    draw_dialogue(options, 1, 1, "used ");
                    draw_dialogue(options, 1, 6, p_move.name);
                    sleep_seconds(1);
                    // was effective
                    if(mod == 2) {
                        wclear(options);
                        draw_dialogue(options, 1, 1, "And was super effective");
                        sleep_seconds(1);
                    }
                    p->mons[0].moves[move_index].mp--;

                } else {
                    draw_dialogue(options, 1, 1, "You dont have MP to use this move");
                }
                sleep_seconds(1);
            }

            if(input == 'e') {
                wclear(options);
                draw_dialogue(options, 1, 1, "w - MOVE UP | s - MOVE DOWN | e - CHOOSE");
                int item_index = get_some_item(p, battle_w_w / 2);
                Item current_item = p->items[item_index];

                //Only doing this way because i will handle only two itens
                if(strcmp(current_item.name, "Potion") == 0 && current_item.quantity > 0) {
                    p->mons[0].current_hp += 10; 
                    p->mons[0].current_hp = p->mons[0].current_hp > p->mons[0].max_hp? p->mons[0].max_hp : p->mons[0].current_hp;
                } else if(strcmp(current_item.name, "MonBall") == 0 && current_item.quantity > 0) {
                    wclear(options);
                    draw_dialogue(options, 1, 1, "You threw a MonBall");
                    sleep_seconds(1);

                    // actual algorithm to calc chance to capture
                    int ball_num = 12;
                    int n = rand() % 256;
                    float f = (random_mon.max_hp * 255 * 4) / (random_mon.current_hp * ball_num);
                    wclear(options);

                    for(int i = 0; i < 3; i++) {
                        draw_dialogue(options, 1, i + 1, ".");
                        sleep_seconds(1);
                    }  

                    if(f >= n) {
                        wclear(options);
                        draw_dialogue(options, 1, 1, "You caught that mon!");
                        sleep_seconds(1);
                        add_mon(p, &random_mon);

                        wclear(options);
                        draw_dialogue(options, 1, 1, random_mon.name);
                        draw_dialogue(options, 1, strlen(random_mon.name) + 1, "was added to your party");
                        break;
                    } else {
                        wclear(options);
                        draw_dialogue(options, 1, 1, "You failed");
                    }


                }
                p->items[item_index].quantity--;
                continue;
            }
            if(input == 'w') {
                wclear(options);
                draw_dialogue(options, 1, 1, "w - MOVE UP | s - MOVE DOWN | f - EXIT | space - CHANGE");
                choose_mon(p, battle_w_w / 2);

                wclear(options);
                draw_dialogue(options, 1, 1, "you changed mon");
                wclear(battle_win);
                box(battle_win, 0, 0);
                wrefresh(battle_win);
                sleep_seconds(1);
                continue;
            }
            
            turn = 1; 

            if(check_if_defeated(&random_mon)) {
                wclear(options);
                draw_dialogue(options, 1, 1, random_mon.name);
                draw_dialogue(options, 1, r_mon_name_len + 1, " was defeated");
                sleep_seconds(2);
                int total_xp = rand() % random_mon.lvl + 1;
                p->mons[0].xp_points += total_xp;

                wclear(options);
                draw_dialogue(options, 1, 1, int_to_string(total_xp));
                draw_dialogue(options, 1, 3, "xp was gained");
                sleep_seconds(2);

                if(lvl_up(&p->mons[0])) {
                   wclear(options);
                   draw_dialogue(options, 1, 1, "you leveled up"); 
                   sleep_seconds(1);
                }
                break;
            }

        } else {   
            // Mon turn
            sleep_seconds(2);
            wclear(options);
            
            int random_move_index = rand() % random_mon.total_moves;
            Move random_move = random_mon.moves[random_move_index];

            if(random_move.mp > 0) {
                random_mon.moves[random_move_index].mp--;
                draw_dialogue(options, 1, 1, random_mon.name);
                draw_dialogue(options, 1, r_mon_name_len + 1, " used ");
                draw_dialogue(options, 1, r_mon_name_len + 7, random_move.name);

                int mod = effectiveness(random_move.type, p->mons[0].type);
                int dano = calc_dmg(random_mon.lvl, random_move.power * 0.6, mod);

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



char* trainer_mons_alive_line(Mon mons[], int t_mons) {
    int line_w = 13;
    char *line = (char *)malloc(line_w);


    int j = 0;
    for(int i = 0; i < line_w; i++) {
        
        if (i % 2 == 1 && j < t_mons) {  // Corrigido para i % 2 == 0
            line[i] = check_if_defeated(&mons[j]) ? 'x' : 'o';
            j++;
        } else {
            line[i] = '-';
        }
    } 
    line[13] = '\0';

    return line;

}

void battle_trainer(Player *p, GameManager gm, Trainer *t) {
    wclear(gm.main_w);
    wclear(gm.dialog);
    wrefresh(gm.main_w);
    wrefresh(gm.dialog);

    int turn = 0;

    srand(time(NULL));
    int battle_w_w = 60;
    int battle_w_h = 20;
    WINDOW *battle_win = newwin(battle_w_h, battle_w_w, 0, 0);

    WINDOW *options = newwin(3, battle_w_w, battle_w_h, 0);
    int trainer_mon_index = 0;

    int sprite_padding_w = 10;
    int sprite_padding_h =  5;

    int r_mon_x = battle_w_w - sprite_padding_w;
    int r_mon_y = 2;

    int r_mon_name_len = strlen(t->mons[trainer_mon_index].name);

    int p_mon_y = battle_w_h - sprite_padding_h;
    int p_mon_x = 2;

    int input;
    wclear(options);
    draw_dialogue(options, 1, 1, "Trainer wants to battle with you!");


    WINDOW *mon_status = newwin(4, 14, p_mon_y - 1, p_mon_x + 10);
    box(mon_status, 0, 0);  

    WINDOW *r_mon_status = newwin(4, 14 , r_mon_y -1 , r_mon_x - 18);
    box(r_mon_status, 0, 0);

    while(party_up(t->mons, t->n_mons)) {
        box(battle_win, 0, 0);
    
        draw_mon(battle_win, &p->mons[0], p_mon_y, p_mon_x);

        wclear(mon_status);
        draw_status(mon_status, &p->mons[0], 1, 1);
        box(mon_status, 0, 0);
        wrefresh(mon_status);
      

        wclear(r_mon_status);
        draw_mon(battle_win, &t->mons[trainer_mon_index], r_mon_y, r_mon_x);
        draw_status(r_mon_status, &t->mons[trainer_mon_index], 1, 1);
        char *mons_line = trainer_mons_alive_line(t->mons, t->n_mons);
        mvwaddstr(battle_win, r_mon_y - 2, r_mon_x - 18, mons_line);
        wrefresh(battle_win);
        box(r_mon_status, 0, 0);
        wrefresh(r_mon_status);
        
        if(!party_up(p->mons, p->n_of_mons)) {
            wclear(options);
            draw_dialogue(options, 1, 1, "All your mons were defeated");
            sleep_seconds(1);
            wclear(options);
            wrefresh(options);
            
            wclear(battle_win);
            wrefresh(battle_win);
            return;
            
        } 

        if(check_if_defeated(&p->mons[0])) {
            wclear(options);
            draw_dialogue(options, 1, 1, "Your mon was defeated");
            sleep_seconds(1);

            choose_mon(p, battle_w_w / 2);
           
            wclear(options);
            draw_dialogue(options, 1, 1, "you changed mon");
            wclear(battle_win);
            box(battle_win, 0, 0);
            wrefresh(battle_win);
            sleep_seconds(1);
            continue;
        }

        // Player turn
        if(turn == 0) {
            sleep_seconds(2);
            wclear(options);
            draw_dialogue(options, 1, 1, "q - Attack | w - Mons | e - Itens | r - Run");

            do {
                input = wgetch(battle_win);
            } while (input != 'q' && input != 'w' && input != 'e' && input != 'r');

            if(input == 'r') {
                wclear(options);
                draw_dialogue(options, 1, 1, "You can't run from a trainers battle");
                
            }
            if(input == 'q') {
                wclear(options);
                draw_dialogue(options, 1, 1, "w - MOVE UP | s - MOVE DOWN | f - CHOOSE");
                int move_index = get_mon_move(&p->mons[0]);
                Move p_move = p->mons[0].moves[move_index];
                wclear(options);

                if(p_move.mp > 0) {
                    int mod = effectiveness(p_move.type, t->mons[trainer_mon_index].type);
                    int dano = calc_dmg(p->mons[0].lvl, p_move.power, mod);
                    apply_dmg(&t->mons[trainer_mon_index], dano); 
                    draw_dialogue(options, 1, 1, "used ");
                    draw_dialogue(options, 1, 6, p_move.name);
                    sleep_seconds(1);
                    // was effective
                    if(mod == 2) {
                        wclear(options);
                        draw_dialogue(options, 1, 1, "And was super effective");
                        sleep_seconds(1);
                    }
                    p->mons[0].moves[move_index].mp--;

                } else {
                    draw_dialogue(options, 1, 1, "You dont have MP to use this move");
                }
                sleep_seconds(1);
            }

            if(input == 'e') {
                wclear(options);
                draw_dialogue(options, 1, 1, "w - MOVE UP | s - MOVE DOWN | e - CHOOSE");
                int item_index = get_some_item(p, battle_w_w / 2);
                Item current_item = p->items[item_index];

                //Only doing this way because i will handle only two itens
                if(strcmp(current_item.name, "Potion") == 0 && current_item.quantity > 0) {
                    p->mons[0].current_hp += 10; 
                    p->mons[0].current_hp = p->mons[0].current_hp > p->mons[0].max_hp? p->mons[0].max_hp : p->mons[0].current_hp;
                    p->items[item_index].quantity--;
                    wclear(options);
                    draw_dialogue(options, 1, 1, "You used a potion and recovered 10 hp");
                    continue;
                } else if(strcmp(current_item.name, "MonBall") == 0 && current_item.quantity > 0) {
                    wclear(options);
                    draw_dialogue(options, 1, 1, "You threw a MonBall");
                    sleep_seconds(1);
                    wclear(options);
                    draw_dialogue(options, 1, 1, "You can't capture another trainer mon!");
                }
            }
            if(input == 'w') {
                wclear(options);
                draw_dialogue(options, 1, 1, "w - MOVE UP | s - MOVE DOWN | f - EXIT | space - CHANGE");
                choose_mon(p, battle_w_w / 2);

                wclear(options);
                draw_dialogue(options, 1, 1, "you changed mon");
                wclear(battle_win);
                box(battle_win, 0, 0);
                wrefresh(battle_win);
                sleep_seconds(1);
                continue;
            }
            
            
            turn = 1; 

            if(check_if_defeated(&t->mons[trainer_mon_index])) {
                wclear(options);
                draw_dialogue(options, 1, 1, t->mons[trainer_mon_index].name);
                draw_dialogue(options, 1, r_mon_name_len + 1, " was defeated");

                sleep_seconds(1);
                if(trainer_mon_index + 1 < t->n_mons){

                    wclear(options);
                    draw_dialogue(options, 1, 1, "Trainer will change his mon");
                    sleep_seconds(1);
                    trainer_mon_index++;
                
                    wclear(battle_win);
                    turn = 0;
                    
                    continue;
                } else {
                    break;
                }
            }

        } else {   
            // Trainer battle turn
            sleep_seconds(2);
            wclear(options);
            
            int random_move_index = rand() % t->mons[trainer_mon_index].total_moves;
            Move random_move = t->mons[trainer_mon_index].moves[random_move_index];

            if(random_move.mp > 0) {
                t->mons[trainer_mon_index].moves[random_move_index].mp--;
                draw_dialogue(options, 1, 1, t->mons[trainer_mon_index].name);
                draw_dialogue(options, 1, r_mon_name_len + 1, " used ");
                draw_dialogue(options, 1, r_mon_name_len + 7, random_move.name);

                int mod = effectiveness(random_move.type, p->mons[0].type);
                int dano = calc_dmg(t->mons[trainer_mon_index].lvl, random_move.power, mod);

                apply_dmg(&p->mons[0], dano);
                sleep_seconds(1);
            }

            clear_input_buffer(battle_win);
            turn = 0;
        }
        
    }

    sleep_seconds(2);
    int total_xp = (rand() % t->mons[trainer_mon_index].lvl + 1) * t->n_mons;
    p->mons[0].xp_points += total_xp;

    wclear(options);
    draw_dialogue(options, 1, 1, int_to_string(total_xp));
    draw_dialogue(options, 1, 3, "xp was gained");
    sleep_seconds(2);

    if(lvl_up(&p->mons[0])) {
        wclear(options);
        draw_dialogue(options, 1, 1, "you leveled up"); 
        sleep_seconds(1);
    }

    wclear(options);
    wrefresh(options);
    
    wclear(battle_win);
    wrefresh(battle_win);
}