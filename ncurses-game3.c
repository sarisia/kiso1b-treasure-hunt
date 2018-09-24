#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>
#include "redraw2.h"
#include "highscore.h"

int main(int argc, char** argv) {
    // declaration vars
    POSITION *player = (POSITION *)malloc(sizeof(POSITION));
    POSITION *monster = (POSITION *)malloc(sizeof(POSITION));
    POSITION *treasure = (POSITION *)malloc(sizeof(POSITION));
    POSITION drink[5];
    POSITION child[10];
    int countMax;
    int cnt = 0;
    int ch;
    int i;
    int stage = 0;
    int score = 0;
    int numTreasure;
    int numDrink;
    int numBarrier;
    int numChild;
    int clear = 0;
    int barrierd = 0;
    SETTING conf;

    // init rand
    srand((unsigned)time(NULL));

    // init ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    // int keypad(WINDOW *, bool);
    keypad(stdscr, TRUE);
    // void wtimeout(WINDOW *, int delay);
    wtimeout(stdscr, 500);

    // init vars
    player->x = player->y = 5;
    monster->x = COLS / 2;
    monster->y = LINES / 2;
    treasure->x = -1;
    treasure->y = -1;
    numTreasure = 0;
    numDrink = 0;
    numChild = 0;

    // read key first
    ch = getch();

    // hook
    countMax = initGame();
    if(!countMax) {
        sleep(5);
        return 0;
    }
    conf = showMenu();
    initBarrier(conf, &numBarrier);

    while(ch != 'q') {
        // init
        mvprintw(1, 2, "Stage: %d", stage + 1);
        mvprintw(2, 2, "Score: %d", score);
        mvprintw(2, 15, "Barrier: %d", numBarrier);
        mvprintw(3, 2, "Count: %3d", countMax - cnt);
        cnt++;
        highScoreDisp(getHighScore(loadHighScore(), score));

        // move monster
        for(int i = 0; i < numChild; i++) colorprint(0, child[i].y, child[i].x, " ");
        mvprintw(monster->y, monster->x, " ");
        getMonsterLocationEx(conf, monster, treasure, &numTreasure, drink, &numDrink);
        // move children
        getChildLocation(conf, monster, treasure, &numTreasure, drink, &numDrink, child, &numChild);
        // print
        for(int i = 0; i < numChild; i++) colorprint(6, child[i].y, child[i].x, "M");
        colorprint(2, monster->y, monster->x, "M");

        // move player
        mvprintw(player->y, player->x, " ");
        getPlayerLocation(player, ch);
        colorprint(1, player->y, player->x, "@");
        if(barrierd) {
            colorprint(5, player->y, player->x, "@");
            barrierd--;
        }

        refresh();

        // judgement
        if(getDrink(player, drink, &numDrink)) cnt -= COLS < LINES ? COLS / 3 : LINES / 3;
        clear += getTreasure(player, treasure, &numTreasure);
        if(cnt == countMax) {
            // cound not get treasures
            pre_gameover(score);
            ch = gameoverEx();
        } else if(encounter(player, monster) || encountChild(player, child, numChild)) {
            if(numBarrier) {
                numBarrier--;
                barrierd = 3;
                continue;
            }
            // caught by monster
            pre_gameover(score);
            ch = gameoverEx();
        } else if(clear == stage + 1) {
            // stage clear
            mvprintw(4, 2, "Gotcha!");
            refresh();
            sleep(2);
            mvprintw(4, 2, "       ");
            stage++;
            score += (countMax - cnt) * conf.score;
            cnt = 0;
            clear = 0;
            barrierd = 0;
            initPlayerLocation(player);
            initMonsterLocation(monster);
            initTreasureLocation(treasure, &numTreasure);
            initDrinkLocation(drink, &numDrink);
            initChildLocation(child, &numChild);
            initBarrier(conf, &numBarrier);
        } else {
            ch = getch();
        }
    }

    endwin();
    return 0;
}

