#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "level.h"

void free_level_data(Level *level) {
    for(int i = 0; i < level->row_count; i++) {
        free(level->level_string[i]);
    }
    free(level);
}

void freeList(Level_row *list){
    Level_row *row_pointer = list;
    while(row_pointer != NULL){
        row_pointer = list->next;
        free(list);
        list = row_pointer;
    } 
} 

Level* loadLevel(FILE *input){
    int next_char;
    int counter = 0;
    int row_count = 1;

    Level_row *row_list = malloc(sizeof(Level_row));
    Level_row *row_pointer = row_list;
    row_pointer->string = calloc(2, sizeof(char));

    //printf("%s %i\n", row_pointer->string, strlen(row_pointer->string));

    while((next_char = fgetc(input)) != EOF){
        if(next_char == '\n'){
            //printf("%s\n", row_pointer->string);
            row_pointer->next = malloc(sizeof(Level_row));
            row_pointer = row_pointer->next;
            row_pointer->string = calloc(2, sizeof(char));
            counter = 0;
            row_count++;
        } else{
            row_pointer->string[counter] = next_char;
            row_pointer->string[counter+1] = '\0';
            //char *new_string = realloc(row_pointer->string, strlen(row_pointer->string)+2);; 
            row_pointer->string = realloc(row_pointer->string, strlen(row_pointer->string)+2);; 
            counter++;
        }
    }
    //last row is empty;
    row_count--;
    free(row_pointer);
    row_pointer = row_list;

    //parse list to 2d char array
    char **level_string = malloc(row_count);
    for(int i = 0; i < row_count; i++) {
        level_string[i] = row_pointer->string;
        row_pointer = row_pointer->next;
    }

    //free list, not needed anymore
    //freeList(row_list);

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
