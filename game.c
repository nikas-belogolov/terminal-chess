


#include <stdlib.h>
#include <stdbool.h>
#include "game.h"

Game* game_create(void) {
	Game* game = (Game*)malloc(sizeof(Game));

	if (game == NULL) return NULL;

	game->is_game_over = false;
	game->turns_until_draw = 100;
	game->current_player = WHITE;

	game->can_black_castle_left = true;
	game->can_black_castle_right = true;
	game->can_white_castle_left = true;
	game->can_white_castle_right = true;

	board_init(game->board);

	return game;
}

bool game_is_over(Game* game) {
	return game->turns_until_draw == 0;
}