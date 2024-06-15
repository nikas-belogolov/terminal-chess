
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"
#include "utils.h"
#include "game.h"

bool is_last_move_initial_two_square_pawn_advance(Game* game) {
    if (game->last_move.from.rank == -1) return false;
    return  (
        game->board[game->last_move.to.rank][game->last_move.to.file] == WHITE_PAWN ||
        game->board[game->last_move.to.rank][game->last_move.to.file] == BLACK_PAWN
        )
        &&
        (abs(game->last_move.from.rank - game->last_move.to.rank) == 2);
}


bool is_legal_pawn_move(Game* game, Move move) {
    int ranks_moved = abs(move.from.rank - move.to.rank);
    int files_moved = abs(move.from.file - move.to.file);

    if (ranks_moved > 2 || files_moved > 1) return false; // Pawns cannot move more than 2 squares or more than one file

    if ((game->current_player == WHITE && (move.from.rank - move.to.rank) > 0) ||
        (game->current_player == BLACK && (move.from.rank - move.to.rank) < 0)) return false; // Pawn cannot move backwards

    int pawn_starting_pos = game->current_player == WHITE ? 1 : 6;

    if (ranks_moved == 2 && (files_moved != 0 || move.from.rank != pawn_starting_pos)) return false; // Illegal two square pawn advance

    if (game->board[move.to.rank][move.to.file] != EMPTY && files_moved == 0) return false; // Illegal move, eating piece moving forwards

    if (game->board[move.to.rank][move.to.file] == EMPTY &&
        files_moved == 1 &&
        !is_last_move_initial_two_square_pawn_advance(game)) return false; // Moving diagonally without en pasant. Illegal move

    return true;
}

bool can_piece_move(Game* game, Position pos) {
	return true;
}

static bool is_legal_piece_move(Game* game, Move move) {
    switch (game->board[move.from.rank][move.from.file]) {
        case BLACK_PAWN:
        case WHITE_PAWN:
            if (!is_legal_pawn_move(game, move)) return false;
            break;
        //case BLACK_ROOK:
        //case WHITE_ROOK:
        //    if (!is_legal_rook_move(game, move)) return false;
        //    break;
        //case BLACK_BISHOP:
        //case WHITE_BISHOP:
        //    if (!is_legal_bishop_move(game, move)) return false;
        //    break;
        //case BLACK_KNIGHT:
        //case WHITE_KNIGHT:
        //    if (!is_legal_knight_move(game, move)) return false;
        //    break;
        //case BLACK_QUEEN:
        //case WHITE_QUEEN:
        //    if (!is_legal_queen_move(game, move)) return false;
        //    break;
        //case BLACK_KING:
        //case WHITE_KING:
        //    if (!is_legal_king_move(game, move)) return false;
        //    break;
    }

    return true;
}

bool is_legal_move(Game* game, Move move) {
    if (move.from.file == move.to.file &&
        move.from.rank == move.to.rank) return false; // Moving piece to the same place

    Piece piece_type_to_move = game->board[move.from.rank][move.from.file];
    Piece place_to_move = game->board[move.to.rank][move.to.file];

    if (!is_current_player_piece(game->current_player, piece_type_to_move)) return false; // Moving not his piece

    if (is_current_player_piece(game->current_player, place_to_move)) return false; // Moving to space occupied by own piece

    if (!is_legal_piece_move(game, move)) return false; // Illegal piece specific move

    //if (will_king_be_in_check(move)) return false; // Illegal move because of check

    return true;
}

bool is_current_player_in_check(Game* game) {
	return false;
}