#include <stdio.h>

typedef struct Level_row Level_row;
typedef struct Level Level;

struct Level_row{
    char *string;
    Level_row *next;
};

struct Level{
    char **level_string;
    int row_count;
};

Level* loadLevel(FILE *input);

void printLevel(Level *level, FILE *game_output);

void freeList(Level_row *list, int row_count);

void free_level_data(Level *level);