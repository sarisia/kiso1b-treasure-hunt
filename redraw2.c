#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "redraw2.h"

void initPlayerLocation(POSITION *player) {
    // hide current player
    mvprintw(player->y, player->x, " ");
    
    // set new player pos and refresh
    player->x = player-> y = 5;
    mvprintw(player->y, player->x, "@");
}

void initMonsterLocation(POSITION *monster) {
    mvprintw(monster->y, monster->x, " ");
    monster->x = COLS / 2;
    monster->y = LINES / 2;
    mvprintw(monster->y, monster->x, "M");
}

void initTreasureLocation(POSITION *treasure, int *numTreasure) {
    mvprintw(treasure->y, treasure->x, " ");
    *numTreasure = 0;
}

void getPlayerLocation(POSITION *player, int key) {
    switch(key) {
        case KEY_LEFT:
            if(player->x > 0) player->x -= 1;
            break;
        case KEY_RIGHT:
            if(player->x < COLS - 1) player->x += 1;
            break;
        case KEY_UP:
            if(player->y > 0) player->y -= 1;
            break;
        case KEY_DOWN:
            if(player->y < LINES - 1) player->y += 1;
            break;
        default:
            break;
    }
}

void getMonsterLocation(POSITION *monster, POSITION *treasure, int *numTreasure) {
    int map[3] = {-1, 0, 1};

    int dx = map[rand() % 3];
    int dy = map[rand() % 3];
    int origX = monster->x;
    int origY = monster->y;
    int dice = rand() % 10;

    if((monster->x > 0 && dx < 0) || (monster->x < COLS - 1 && dx > 0)) {
        monster->x += dx;
    }
    if((monster->y > 0 && dy < 0) || (monster->y < LINES - 1 && dy > 0)) {
        monster->y += dy;
    }

    // monster cannot get a treasure. stay there
    if(*numTreasure == 1 && monster->x == treasure->x && monster->y == treasure->y) {
        monster->x = origX;
        monster->y = origY;
    }

    // monster drops a treasure
    if(!dice && !*numTreasure && (monster->x != origX || monster->y != origY)) {
        treasure->x = origX;
        treasure->y = origY;
        *numTreasure = 1;
        mvprintw(treasure->y, treasure->x, "T");
    }
}

int getTreasure(POSITION *player, POSITION *treasure, int *numTreasure) {
    if(*numTreasure == 1 && player->x == treasure->x && player->y == treasure->y) {
        mvprintw(treasure->y, treasure->x, " ");
        *numTreasure = 0;
        return 1;
    }

    return 0;
}

int encounter(POSITION *player, POSITION *monster) {
    if(player->x == monster->x && player->y == monster->y) return 1;
    return 0;
}

char gameover() {
    mvprintw(LINES / 2, (COLS -  9) / 2, "GAME OVER");
    refresh();
    sleep(2);
    
    return 'q';
}
