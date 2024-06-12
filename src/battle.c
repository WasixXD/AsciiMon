#include "battle.h"
#include "world.h"



void battle(Player *p) {
    int battle_w_w = 40;
    int battle_w_h = 20;
    WINDOW *battle_win = newwin(battle_w_h, battle_w_w, 0, 0);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 8; j++) {
            mvwaddch(battle_win, i, j, p->mons[0].sprite[i][j]);
        }
    }
    wrefresh(battle_win);
    int input;
    while(1) {
        input = wgetch(battle_win);
        if(input == 'f') {
            break;
        }
    }
}


