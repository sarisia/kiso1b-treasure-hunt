#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include <ncurses.h>
#include "redraw2.h"

#define AABUF 100

typedef struct _aa {
    int width;
    int height;
    char fallback[AABUF];
    char body[20][AABUF];
} AA;

typedef struct _setting {
    int treasure;
    int drink;
    int child;
    int barrier;
    float score;
} SETTING;

typedef struct _level {
    char levelname[10];
    int lendesc;
    char description[10][40];
    struct _setting config;
} LV;

int getHighScore(int highScore, int score);
int loadHighScore();
void saveHighScore(int score);
void highScoreDisp(int highScore);

// orig functions
int initGame();
SETTING showMenu();
void showDescription(int, int, int);
int artcX(int);
int artcY(int);
int strcX(int);
void colorprint(int, int, int, char *, ...);
int _initART();
int _initLEVEL();
void printART(int, int, int, int);
void pre_gameover(int);

void initDrinkLocation(POSITION *, int *);
void initChildLocation(POSITION *, int *);
void initBarrier(SETTING, int *);
void getChildLocation(SETTING, POSITION *, POSITION *, int *, POSITION *, int *, POSITION *, int *);
// port from redraw2.c
void getMonsterLocationEx(SETTING, POSITION *, POSITION *, int *, POSITION *, int *);
int getDrink(POSITION *, POSITION *, int *);
int encountChild(POSITION *, POSITION *, int);
char gameoverEx();
int prand();

#endif
