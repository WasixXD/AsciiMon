#ifndef BATTLE_H
#define BATTLE_H

#include "player.h"
#include "world.h"


void battle(Player *p, GameManager gm);

int effectiveness(char *move_type, char *mon_type);

void battle_trainer(Player *p, GameManager gm, Trainer *t);


#endif