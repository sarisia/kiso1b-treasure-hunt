#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "highscore.h"

int getHighScore(int highScore, int score) {
    return score > highScore ? score : highScore;
}

int loadHighScore() {
    int score, ret;
    FILE *f = fopen("highscore.txt", "r");
    if(f == NULL) return 0;

    ret = fscanf(f, "%d", &score);
    return ret == 1 ? score : 0;
}

void saveHighScore(int score) {
    FILE *f = fopen("highscore.txt", "w");
    if(f == NULL) return;

    fprintf(f, "%d\n", score);
}

void highScoreDisp(int highScore) {
    mvprintw(1, COLS / 2 + 2, "HI SCORE: %d", highScore);
}

// =====================================================================

static AA ART[20];
static LV LEVEL[4];
static int lenLV;
static int centerX;
static int centerY;

int initGame() {
    if(LINES < 25 || COLS < 90) {
        mvprintw(1, 1, "Terminal size too small.");
        mvprintw(2, 1, "Make a window bigger and try again.");
        refresh();
        return 0;
    }

    // init terminal color mode
    if(has_colors()) {
        start_color();
        // Pair 0: while-black (normal)
        init_pair(0, COLOR_WHITE, COLOR_BLACK);
        // Pair 1: black-white (selected)
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        // Pair 2: yellow-red (monster)
        init_pair(2, COLOR_YELLOW, COLOR_RED);
        // Pair 3: black-yellow (treasure)
        init_pair(3, COLOR_BLACK, COLOR_YELLOW);
        // Pair 4: white-blue (POT)
        init_pair(4, COLOR_WHITE, COLOR_CYAN);
        // Pair 5: yellow-yellow (barrier)
        init_pair(5, COLOR_YELLOW, COLOR_YELLOW);
        // Pair 6: yellow-magenta (child)
        init_pair(6, COLOR_YELLOW, COLOR_MAGENTA);
    }

    // init pos
    centerX = COLS / 2;
    centerY = LINES / 2;

    // init levels
    if(_initLEVEL()) {
        mvprintw(1, 1, "Failed to load LEVEL.txt");
        mvprintw(2, 1, "Necessary file is missing. Exit.");
        refresh();
        return 0;
    }

    // init ART
    if(_initART()) {
        mvprintw(1, 1, "Failed to load ART.txt");
        mvprintw(2, 1, "Necessary file is missing. Exit.");
        refresh();
        return 0;
    }

    return COLS + LINES;
}

int artcX(int aa) {
    return (COLS - ART[aa].width) / 2;
}

int artcY(int aa) {
    return (LINES - ART[aa].height) / 2;
}

int strcX(int len) {
    return (COLS - len) / 2;
}

SETTING showMenu() {
    int cur = 0;
    int sel;
    int blink = 1;

    printART(0, 3, artcY(3) - 6, artcX(3));

    sel = getch();
    while(sel != '\n') {
        if(sel == 'q') {
            endwin();
            exit(0);
        }
        // move cursor
        if(sel == KEY_DOWN) {
            cur = (cur + 1) % lenLV;
            blink = 1;
        }
        if(sel == KEY_UP) {
            cur = !cur ? (lenLV - 1) : (cur - 1);
            blink = 1;
        }

        // print menu
        for(int i = 0; i < lenLV; i++) {
            if(i == cur) {
                colorprint(blink, centerY + 2 + (i + 1) * 2, strcX(strlen(LEVEL[i].levelname)) - 15, "%s", LEVEL[i].levelname);
                if(blink) blink = 0;
                else blink = 1;
            }
            else colorprint(0, centerY + 2 + (i + 1) * 2, strcX(strlen(LEVEL[i].levelname)) - 15, "%s", LEVEL[i].levelname);
        }

        showDescription(cur, centerY + 5, centerX);
        refresh();
        sel = getch();
    }

    clear();
    return LEVEL[cur].config;
}

void showDescription(int lv, int y, int x) {
    for(int i = 0; i < 10; i++) colorprint(0, y + i, x, "                                             ");
    for(int i = 0; i < LEVEL[lv].lendesc; i++) colorprint(0, y + i, x, "%s", LEVEL[lv].description[i]);
}

void colorprint(int pair, int y, int x, char *format, ...) {
    // wrapper of mvprintw that can output colored chars
    int color = COLOR_PAIR(pair);
    va_list args;

    va_start(args, format);

    move(y, x);
    attron(color);
    vwprintw(stdscr, format, args);
    attroff(color);

    va_end(args);
}

int _initART() {
    // construct AsciiART assets
    int aas, w, h;
    char _s[AABUF];
    FILE *fp = fopen("ART.txt", "r");
    if(fp == NULL) return -1;

    fscanf(fp, "%d", &aas);
    
    for(int i = 0; i < aas; i++) {
        fscanf(fp, "%d%d", &h, &w); // "%d%d " として空白を読み飛ばすと次の行も読み飛ばされて崩れる
        fgets(_s, AABUF, fp); // 行末まで空読みが正解。改行文字で止まってくれる。やらないと次のfgetsが死ぬ
        fgets(ART[i].fallback, AABUF, fp);
        ART[i].height = h;
        ART[i].width = w;
        for(int j = 0; j < h; j++) fgets(ART[i].body[j], AABUF, fp);
    }

    return 0;
}

int _initLEVEL() {
    int lvs, ld;
    char _s[40];
    SETTING conf;
    FILE *fp = fopen("LEVEL.txt", "r");
    if(fp == NULL) return -1;

    fscanf(fp, "%d", &lvs);
    lenLV = lvs;
    fgets(_s, 40, fp);
    fgets(_s, 40, fp);

    for(int i = 0; i < lvs; i++) {
        fgets(LEVEL[i].levelname, 10, fp);
        fscanf(fp, "%d%d%d%d%d%f", &ld, &conf.treasure, &conf.drink, &conf.child, &conf.barrier, &conf.score);
        LEVEL[i].lendesc = ld;
        LEVEL[i].config = conf;
        fgets(_s, 40, fp);
        for(int j = 0; j < ld; j++) fgets(LEVEL[i].description[j], 40, fp);
        fgets(_s, 40, fp);
    }

    return 0;
}

void printART(int pair, int aa, int y, int x) {
    for(int i = 0; i < ART[aa].height; i++) {
        colorprint(pair, y + i, x, "%s", ART[aa].body[i]);
    }
}

void pre_gameover(int score) {
    int highScore = getHighScore(loadHighScore(), score);
    saveHighScore(highScore);
    highScoreDisp(highScore);
}

void initDrinkLocation(POSITION *drink, int *numDrink) {
    for(int i = 0; i < *numDrink; i++) {
        mvprintw(drink[i].y, drink[i].x, " ");
    }
    *numDrink = 0;
}

void initChildLocation(POSITION *child, int *numChild) {
    for(int i = 0; i < *numChild; i++) {
        mvprintw(child[i].y, child[i].x, " ");
    }
    *numChild = 0;
}

void initBarrier(SETTING conf, int *numBarrier) {
    *numBarrier = conf.barrier;
}

void getChildLocation(SETTING conf, POSITION *monster, POSITION *treasure, int *numTreasure, POSITION *drink, int *numDrink, POSITION *child, int *numChild) {
    int map[3] = {-1, 0, 1};
    
    int dy, dx;
    int origY, origX;
    int spawnDice = prand();
    int dice;

    // spawn
    if(spawnDice < 20 && *numChild < conf.child) {
        child[*numChild].x = monster->x;
        child[*numChild].y = monster->y;
        *numChild = *numChild + 1;
    }

    for(int i = 0; i < *numChild; i++) {
        dy = map[rand() % 3];
        dx = map[rand() % 3];
        origY = child[i].y;
        origX = child[i].x;

        if((child[i].x > 0 && dx < 0) || (child[i].x < COLS - 1 && dx > 0)) {
            child[i].x += dx;
        }
        if((child[i].y> 0 && dy < 0) || (child[i].y < LINES - 1 && dy > 0)) {
            child[i].y += dy;
        }

        // children dont need to avoid monsters, or other children,
        // but need to avoid treasure and drinks.
        if(*numTreasure == 1 && child[i].x == treasure->x && child[i].y == treasure->y) {
            child[i].x = origX;
            child[i].y = origY;
        }
        // same for drinks
        for(int j = 0; j < *numDrink; j++) {
            if(child[i].x == drink[j].x && child[i].y == drink[j].y) {
                child[i].x = origX;
                child[i].y = origY;
            }
        }
    }
}

// port from redraw2.c
void getMonsterLocationEx(SETTING conf, POSITION *monster, POSITION *treasure, int *numTreasure, POSITION *drink, int *numDrink) {
    int map[3] = {-1, 0, 1};

    int dx = map[rand() % 3];
    int dy = map[rand() % 3];
    int origX = monster->x;
    int origY = monster->y;
    int dice1 = prand();
    int dice2 = prand();

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
    // same for drinks
    for(int i = 0; i < *numDrink; i++) {
        if(monster->x == drink[i].x && monster->y == drink[i].y) {
            monster->x = origX;
            monster->y = origY;
        }
    }

    // monster drops a treasure
    if(dice1 < conf.treasure && !*numTreasure && (monster->x != origX || monster->y != origY)) {
        treasure->x = origX;
        treasure->y = origY;
        *numTreasure = 1;
        colorprint(3, treasure->y, treasure->x, "T");
    }
    else if(dice2 < conf.drink && *numDrink < 5) {
        drink[*numDrink].x = origX;
        drink[*numDrink].y = origY;
        *numDrink = *numDrink + 1;
        colorprint(4, origY, origX, "E");
    }
}

int getDrink(POSITION *player, POSITION *drink, int *numDrink) {
    for(int i = 0; i < *numDrink; i++) {
        if(player->x == drink[i].x && player->y == drink[i].y) {
            colorprint(0, drink[i].y, drink[i].x, " ");
            drink[i].x = drink[*numDrink - 1].x;
            drink[i].y = drink[*numDrink - 1].y;
            *numDrink = *numDrink - 1;
            return 1;
        }
    }

    return 0;
}

int encountChild(POSITION *player, POSITION *child, int numChild) {
    for(int i = 0; i < numChild; i++) {
        if(player->x == child[i].x && player->y == child[i].y) return 1;
    }

    return 0;
}

char gameoverEx() {
    printART(0, 4, artcY(4), artcX(4));
    refresh();
    sleep(4);
    
    return 'q';
}

int prand() {
    return rand() % 100;
}
