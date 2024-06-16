#include "board.h"
#include "utils.h"
#include "game.h"
#include <stdbool.h>

bool is_white_piece(Piece piece) {
	return piece >= WHITE_KING && piece <= WHITE_PAWN;
}

bool is_black_piece(Piece piece) {
	return piece >= BLACK_KING && piece <= BLACK_PAWN;
}


bool is_current_player_piece(Player current_player, Piece piece) {
	return
		(current_player == WHITE && is_white_piece(piece)) ||
		(current_player == BLACK && is_black_piece(piece));
}

bool can_current_player_castle(Game* game) {
	return
		(game->current_player == WHITE && game->can_white_castle_left && game->can_white_castle_right) ||
		(game->current_player == BLACK && game->can_black_castle_left && game->can_black_castle_right);
}

bool is_legal_position(Position pos) {
	return pos.file >= 0 && pos.file <= 7 && pos.rank >= 0 && pos.rank <= 7;
}

