CFLAGS = "-std=c99"

ncurses-game3: ncurses-game3.c redraw2.c highscore.c
	@gcc $(CFLAGS) -o ncurses-game3 ncurses-game3.c redraw2.c highscore.c -lncurses
