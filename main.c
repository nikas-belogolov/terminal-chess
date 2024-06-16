// terminal-chess.cpp : Defines the entry point for the application.
//

#include "stdio.h"
#include "game.h"
#include "locale.h"
#include "io.h"

int main()
{
	setlocale(LC_ALL, "en_US.UTF-8");
	#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	#endif
	
	Game* game = game_create();
	
	board_print(game);

	while (!game_is_over(game)) {
		clear_screen();
		board_print(game);
		printf("\nPlayer to move: %s\n", game->current_player == WHITE ? "white" : "black");

		board_move(game, get_move(game));
	}

	//clear_screen();
	//board_print(game);
	game_print_winner(game);

	return 0;
}
