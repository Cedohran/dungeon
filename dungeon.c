#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <getopt.h>
#include "dungeon.h"

void free_enemy_list(Unit *list) {
    if(list == NULL) return;
    Unit *pointer = list;
    while(pointer != NULL){
        pointer = list->next_unit;
        free(list);
        list = pointer;
    }
}

void loadUnits(Unit *player, Unit *enemy_list, Level *level){
    Unit *enemy_pointer = enemy_list;
    char *row;

    for(int i = 0; i < level->row_count; i++){
        row = level->level_string[i];

        for(int j = 0; j < strlen(row); j++){
            if((row[j] == '<' || row[j] == '>' || row[j] == '^' || row[j] == 'v')) {
                enemy_pointer->x_coord = j;
                enemy_pointer->y_coord = i;
                enemy_pointer->icon = row[j];
                enemy_pointer->next_unit = malloc(sizeof(Unit));
                enemy_pointer->next_unit->next_unit = NULL;
                enemy_pointer = enemy_pointer->next_unit;
            }
            if(row[j] == 'S'){
                player->x_coord = j;
                player->y_coord = i;
                player->next_unit = NULL;
                player->icon = row[j];
            }
        }
        
    }
}

int main(int argc, char **argv){
    FILE *level_file;
    FILE *player_input = stdin;
    FILE *game_output = stdout;
    char move_input;
    Level *level;
    Unit *player = malloc(sizeof(player));;
    Unit *enemy_list = malloc(sizeof(Unit));
    enemy_list->icon = 'f';
    enemy_list->next_unit = NULL;
    Unit *enemy_pointer;

    //Argumente
    int opt;
    while( (opt = getopt(argc, argv, ":i:o:")) != -1){
        switch(opt){
            case 'i': 
                if((player_input = fopen(optarg, "r")) == NULL){
                    printf("Error, wrong parameter %s.\n", optarg);
                    return(3);
                } 
                break;
            case 'o':
                if((game_output = fopen(optarg, "w")) == NULL){
                    printf("Error, wrong parameter %s.\n", optarg);
                    return(3);
                } 
                break;
            case':':
                printf("Error, missing parameter.\n");
                return(3);
                break;
            case'?':
                printf("Error, unkown option: %c\n", optopt);
                return(3);
                break;
        }
    }
    //Ueberpruefung der restlichen Argumente
    argc -= optind;
    argv += optind;

    if(argc == 1 && (strstr(*argv, ".txt") != NULL)){
        if((level_file = fopen(*argv, "r")) == NULL){
            printf("%s could not be opened.", *argv);
            return 1;
        } 
    } 
    else if(argc == 1 && (strstr(*argv, ".txt") == NULL)){
        printf("%s could not be read.", *argv);
        return 2;
    } 
    else {
        level_file = fopen("level/1.txt", "r");
    }

    //initialize level and units
    level = loadLevel(level_file);
    loadUnits(player, enemy_list, level);
    enemy_pointer = enemy_list;

    printLevel(level, game_output);

    int turn = 1;
    while((move_input = getc(player_input)) != EOF){
        //fscanf(player_input, " %c", &move_input);
        if(movePlayer(player, move_input, level) == 1) {
            printLevel(level, game_output);
            printf("Gewonnen!\n");
            free_level_data(level);
            //free_enemy_list(enemy_list);
            //free(player);
            return 0;
        }
        if(enemy_list->icon != 'f') {
            while(enemy_pointer != NULL){
                if(moveEnemy(enemy_pointer, level) == 1){
                    printLevel(level, game_output);
                    printf("Du wurdest von einem Monster gefressen.\n");
                    free_level_data(level);
                    free_enemy_list(enemy_list);
                    free(player);
                    return 0;
                }
                enemy_pointer = enemy_pointer->next_unit;
            }
        }
        enemy_pointer = enemy_list;
        fprintf(game_output, "%i %c\n", turn, move_input);
        printLevel(level, game_output);
        turn++;
    }

    free_level_data(level);
    free_enemy_list(enemy_list);
    free(player);
    return 0;
}