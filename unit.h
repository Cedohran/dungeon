#include "level.h"

typedef struct Unit Unit;

struct Unit{
    int x_coord;
    int y_coord;
    Unit *next_unit;
    char icon;
};

int movePlayer(Unit *u, char direction, Level *level);

int moveEnemy(Unit *u, Level *level);