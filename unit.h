#include "level.h"

typedef struct Unit Unit;

struct Unit{
    int x_coord;
    int y_coord;
    Unit *next_unit;
    char icon;
};

int move_and_set_player(Unit *u, char direction, Level *level);

int move_enemy(Unit *u, Level *level);

void set_enemies(Unit *enemy_list, Level *level);