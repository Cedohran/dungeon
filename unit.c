#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "unit.h"

//return values:  -1 - player can't move because of wall, 0 - player moves, 1 - player exits the dungeon
int move_and_set_player(Unit *u, char direction, Level *level) {
    int move_x = 0;
    move_x = u->x_coord;
    int move_y = 0;
    move_y = u->y_coord;
    char next_field;
    
    //check direction for movement
    switch (direction){
        case 'w': 
            move_y--;
            break;
        case 's': 
            move_y++;
            break;
        case 'a': 
            move_x--;
            break;
        case 'd': 
            move_x++;
            break;
    }
    //look at next destination
    next_field = level->level_string[move_y][move_x];
    
    //# is blocking the way, player can not move
    if(next_field == '#') {
        return -1;
    }
    //field is empty -> player can move
    if(next_field == ' ') {
        //move player icon
        level->level_string[u->y_coord][u->x_coord] = ' ';
        level->level_string[move_y][move_x] = u->icon;
        u->x_coord = move_x;
        u->y_coord = move_y;
        return 0;
    } //if player steps onto an exit field
    else if(next_field == 'A') {
        level->level_string[u->y_coord][u->x_coord] = ' ';
        u->x_coord = move_x;
        u->y_coord = move_y;
        return 1;
    }//if player moves into enemy, step on him
    else if(next_field == '<' || next_field == '>' || next_field == '^' || next_field == 'v') {
        level->level_string[u->y_coord][u->x_coord] = ' ';
        level->level_string[move_y][move_x] = u->icon;
        u->x_coord = move_x;
        u->y_coord = move_y;
        return 0;
    }
    //default return
    return -1;
}

//return values:  -1 - enemy turns around, 0 - enemy moves, 1 - enemy eats player
int move_enemy(Unit *u, Level *level) {
    int move_x = 0;
    move_x = u->x_coord;
    int move_y = 0;
    move_y = u->y_coord;
    char next_field;
    //check if player is on top
    int player_on_top = 0;
    if(level->level_string[u->y_coord][u->x_coord] == 'S') player_on_top = 1;
    
    //check direction for movement
    switch (u->icon){
        case '<': 
            move_x--;
            break;
        case '>': 
            move_x++;
            break;
        case 'v': 
            move_y++;
            break;
        case '^': 
            move_y--;
            break;
    }
    //look at next destination
    next_field = level->level_string[move_y][move_x];
    if(!player_on_top) level->level_string[u->y_coord][u->x_coord] = ' ';

    //field is empty -> enemy can move
    if(next_field == ' ') {
        //move enemy icon
        u->x_coord = move_x;
        u->y_coord = move_y;
        return 0;
    }else if(next_field == '<' || next_field == '>' || next_field == '^' || next_field == 'v') {
        u->x_coord = move_x;
        u->y_coord = move_y;
        return 0;
    }
    //wall or door, enemy turns around
    else if(next_field == '#' || next_field == 'A') {
        switch (u->icon){
            case '<': 
                u->icon = '>';
                break;
            case '>': 
                u->icon = '<';
                break;
            case 'v': 
                u->icon = '^';
                break;
            case '^': 
                u->icon = 'v';
                break;
        }
        //kill player if on top while turning around
        if(player_on_top) return 1;
        return -1;
    }//enemy eats player
    else if(next_field == 'S'){
        //move enemy icon
        //do no errase if player was on top
        u->x_coord = move_x;
        u->y_coord = move_y;
        return 1;
    }
    return -1;
}

//sets enemy icons on level
void set_enemies(Unit *enemy_list, Level *level){
    Unit *enemy_pointer = enemy_list;
    char field_char;
    while(enemy_pointer != NULL) {
        field_char = level->level_string[enemy_pointer->y_coord][enemy_pointer->x_coord];
        if ((field_char != '<') && (field_char != '>') && (field_char != '^') && (field_char != 'v')){
            level->level_string[enemy_pointer->y_coord][enemy_pointer->x_coord] = enemy_pointer->icon;
        }
        enemy_pointer = enemy_pointer->next_unit;
    }
}