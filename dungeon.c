#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <getopt.h>
#include "dungeon.h"

void free_enemy_list(Unit *list) {
    if(list->icon == 'f'){ 
        free(list);
        return;
    }
    Unit *pointer = list;
    Unit *pointer2;
    while(pointer != NULL){
        pointer2 = pointer->next_unit;
        free(pointer);
        pointer = pointer2;
    }
}

void loadUnits(Unit *player, Unit *enemy_list, Level *level){
    Unit *enemy_pointer = enemy_list;
    char *row;

    for(int i = 0; i < level->row_count; i++){
        //copy row
        row = malloc(strlen(level->level_string[i])+1);
        strcpy(row, level->level_string[i]);

        for(int j = 0; j < strlen(row); j++){
            if((row[j] == '<' || row[j] == '>' || row[j] == '^' || row[j] == 'v')) {
                if(enemy_pointer->next_unit == NULL && enemy_pointer->icon != 'f'){
                    enemy_pointer->next_unit = malloc(sizeof(Unit));
                    enemy_pointer->next_unit->next_unit = NULL;
                    enemy_pointer = enemy_pointer->next_unit;
                }
                enemy_pointer->x_coord = j;
                enemy_pointer->y_coord = i;
                enemy_pointer->icon = row[j];
            }
            if(row[j] == 'S'){
                player->x_coord = j;
                player->y_coord = i;
                player->next_unit = NULL;
                player->icon = row[j];
            }
        }
        free(row);
    }
}

int main(int argc, char **argv){
    FILE *level_file;
    FILE *player_input = stdin;
    FILE *game_output = stdout;
    //Argumente
    int opt;
    while( (opt = getopt(argc, argv, ":i:o:")) != -1){
        switch(opt){
            case 'i': 
                if(player_input != stdin){
                    fprintf(stderr, "Error, duplicate option.\n");
                    //close if necessary
                    if(game_output != stdout) fclose(game_output);
                    fclose(player_input);
                    return 3;
                }
                if((player_input = fopen(optarg, "r")) == NULL){
                    fprintf(stderr, "Error, could not open %s\n", optarg);
                    //close if necessary
                    if(game_output != stdout) fclose(game_output);
                    return 1;
                } 
                break;
            case 'o':
                if(game_output != stdout){
                    fprintf(stderr, "Error, duplicate option.\n");
                    //close if necessary
                    if(player_input != stdin) fclose(game_output);
                    fclose(game_output);
                    return 3;
                }
                if((game_output = fopen(optarg, "w")) == NULL){
                    fprintf(stderr, "Error, could not open %s\n", optarg);
                    //close if necessary
                    if(player_input != stdin) fclose(player_input);
                    return 1;
                } 
                break;
            case':':
                fprintf(stderr, "Error, missing parameter.\n");
                return(3);
                break;
            case'?':
                fprintf(stderr, "Error, unkown option: %c\n", optopt);
                return(3);
                break;
        }
    }
    //Ueberpruefung der restlichen Argumente
    argc -= optind;
    argv += optind;
    
    //too many arguments
    if(argc > 1){
        fprintf(stderr, "Error, too many arguments.\n");
        //close if necessary
        if(game_output != stdout) fclose(game_output);
        if(player_input != stdin) fclose(player_input);
        return 3;
    }
    //check for level_file argument
    if(argc == 1){
        //can level_file be opened?
        if((level_file = fopen(*argv, "r")) == NULL){
            fprintf(stderr, "Error, could not open %s\n", *argv);
            //close if necessary
            if(game_output != stdout) fclose(game_output);
            if(player_input != stdin) fclose(player_input);
            return 1;
        }//check if level_file is readable 
        else if(getc(level_file) == EOF){
            fprintf(stderr, "Error, could not read level file.\n");
            if(game_output != stdout) fclose(game_output);
            if(player_input != stdin) fclose(player_input);
            fclose(level_file);
            return 2;
        } else {
            rewind(level_file);
        }
    }//no argument, open default level 1 
    else {
        level_file = fopen("level/1.txt", "r");
    }
    //test whether control input file can be read
    if(player_input != stdin) {
        if(getc(player_input) == EOF){
            fprintf(stderr, "Error, could not read control input.\n");
            //close
            if(game_output != stdout) fclose(game_output);
            fclose(player_input);
            fclose(level_file);
            return 2;
        } else {
            rewind(player_input);
        }
    }

    //initialize level and units
    Level *level;
    Unit *player = malloc(sizeof(Unit));;
    Unit *enemy_list = malloc(sizeof(Unit));
    enemy_list->icon = 'f';
    enemy_list->next_unit = NULL;
    Unit *enemy_pointer;

    level = loadLevel(level_file);
    loadUnits(player, enemy_list, level);
    enemy_pointer = enemy_list;
    printLevel(level, game_output);

    //main game loop
    int turn = 1;
    char move_input;
    int got_eaten = 0;
    int escaped = 0;
    while(fscanf(player_input, " %c", &move_input) != EOF){
        if(move_and_set_player(player, move_input, level) == 1) escaped = 1;
        if(enemy_list->icon != 'f') {
            while(enemy_pointer != NULL){
                if(move_enemy(enemy_pointer, level) == 1) got_eaten = 1;
                enemy_pointer = enemy_pointer->next_unit;
            }
            set_enemies(enemy_list, level);
            enemy_pointer = enemy_list;
        }
        //look for game end
        if(escaped) {
            fprintf(game_output, "%i %c\n", turn, move_input);
            printLevel(level, game_output);
            fprintf(game_output, "Gewonnen!\n");
            //close and free
            fclose(level_file);
            fclose(game_output);
            fclose(player_input);
            free_level_data(level);
            free_enemy_list(enemy_list);
            free(player);
            return 0;
        }
        if(got_eaten){
            fprintf(game_output, "%i %c\n", turn, move_input);
            printLevel(level, game_output);
            fprintf(game_output, "Du wurdest von einem Monster gefressen.\n");
            //close and free
            fclose(level_file);
            fclose(game_output);
            fclose(player_input);
            free_level_data(level);
            free_enemy_list(enemy_list);
            free(player);
            return 0;
        }

        //default turn end output
        fprintf(game_output, "%i %c\n", turn, move_input);
        printLevel(level, game_output);
        turn++;
    }

    //EOF, close and free
    fclose(level_file);
    fclose(game_output);
    fclose(player_input);
    free_level_data(level);
    free_enemy_list(enemy_list);
    free(player);
    return 0;
}