CFLAGS = "-std=c99"

.PHONY: ncurses-game3

ncurses-game3:
	@gcc $(CFLAGS) -lncurses -o ncurses-game3 ncurses-game3.c redraw2.c highscore.c
