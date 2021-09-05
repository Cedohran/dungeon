#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "level.h"

void free_level_data(Level *level) {
    for(int i = 0; i < level->row_count; i++) {
        free(level->level_string[i]);
    }
    free(level->level_string);
    free(level);
}

void freeList(Level_row *list, int row_count){
    Level_row *row_pointer = list;
    for(int i = 0; i < row_count; i++){
        row_pointer = list->next;
        free(list->string);
        free(list);
        list = row_pointer;
    }
    /*while(row_pointer != NULL){
        row_pointer = list->next;
        free(list);
        list = row_pointer;
    } */
} 

Level* loadLevel(FILE *input){
    int next_char;
    int counter = 0;
    int row_count = 1;

    Level_row *row_list = malloc(sizeof(Level_row));
    row_list->next = NULL;
    row_list->string = malloc(2 * sizeof(char));
    row_list->string[0] = ' ';
    row_list->string[1] = '\0';
    Level_row *row_pointer = row_list;

    while((next_char = fgetc(input)) != EOF){
        if(next_char == '\n'){
            //printf("%s\n", row_pointer->string);
            row_pointer->next = malloc(sizeof(Level_row));
            row_pointer->next->next = NULL;
            row_pointer = row_pointer->next;
            //next row
            row_pointer->string = malloc(2 * sizeof(char));
            row_pointer->string[0] = ' ';
            row_pointer->string[1] = '\0';
            counter = 0;
            row_count++;
        } else{
            //HIER IST DER FEHLER
            row_pointer->string[counter] = next_char;
            row_pointer->string[counter+1] = '\0';
            //char *new_string = realloc(row_pointer->string, strlen(row_pointer->string)+2);; 
            row_pointer->string = realloc(row_pointer->string, strlen(row_pointer->string)+ (2 * sizeof(char))); 
            counter++;
        }
    }
    //check if level is valid
    /*if(row_count < 3) {
        return NULL;
    }*/
    //last row is empty
    row_count--;
    free(row_pointer->string);
    row_pointer->string = NULL;
    free(row_pointer);
    row_pointer = NULL;

    //parse list to 2d char array
    row_pointer = row_list;
    char **level_string = malloc(row_count * sizeof(char *));
    for(int i = 0; i < row_count; i++) {
        level_string[i] = malloc(strlen(row_pointer->string)+1);
        strcpy(level_string[i], row_pointer->string);
        row_pointer = row_pointer->next;
    }

    //free list, not needed anymore
    freeList(row_list, row_count);

    //initialize level
    Level *level = malloc(sizeof(Level));
    level->level_string = level_string;
    level->row_count = row_count;
    return(level);
}

void printLevel(Level *level, FILE *game_output) {
    for(int i = 0; i < level->row_count; i++){
        fprintf(game_output, "%s\n", level->level_string[i]);
    }
}
