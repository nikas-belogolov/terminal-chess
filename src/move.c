
#include "board.h"
#include "game.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

static bool is_last_move_initial_two_square_pawn_advance(Game* game) {
    if (game->last_move.from.rank == -1) return false;
    return  (
        game->board[game->last_move.to.rank][game->last_move.to.file] == WHITE_PAWN ||
        game->board[game->last_move.to.rank][game->last_move.to.file] == BLACK_PAWN
        )
        &&
        (abs(game->last_move.from.rank - game->last_move.to.rank) == 2);
}

static bool is_legal_pawn_move(Game* game, Move move) {
    int ranks_moved = abs(move.from.rank - move.to.rank);
    int files_moved = abs(move.from.file - move.to.file);

    // Pawns cannot move more than 2 ranks, more than one file or 0 ranks
    if (ranks_moved > 2 || files_moved > 1 || ranks_moved == 0) return false; 

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

static bool is_legal_rook_move(Game* game, Move move) {
    if (move.from.rank != move.to.rank && move.from.file != move.to.file) return false; // Not horizontal or vertical move

    if (move.from.file == move.to.file) { // Vertical movement

        int min_rank = min(move.from.rank, move.to.rank);
        int max_rank = max(move.from.rank, move.to.rank);

        for (int i = min_rank + 1; i < max_rank; i++) {
            if (game->board[i][move.to.file] != EMPTY) return false;
        }
    }
    else { // Horizontal movement
        int min_file = min(move.from.file, move.to.file);
        int max_file = max(move.from.file, move.to.file);

        for (int i = min_file + 1; i < max_file; i++) {
            if (game->board[move.to.rank][i] != EMPTY) return false;
        }
    }

    if (game->current_player == WHITE) {
        if (move.from.file == 0)  game->can_white_castle_left = false;
        else if (move.from.file == 7) game->can_white_castle_right = false;
    }
    else {
        if (move.from.file == 0) game->can_black_castle_left = false;
        else if (move.from.file == 7) game->can_black_castle_right = false;
    }

    return true;
}

static bool is_legal_bishop_move(Game* game, Move move) {

    int rank_diff = abs(move.from.rank - move.to.rank);
    int file_diff = abs(move.from.file - move.to.file);

    if (rank_diff != file_diff) return false;

    int rank_step = (move.to.rank - move.from.rank) / rank_diff;
    int file_step = (move.to.file - move.from.file) / file_diff;

    for (int rank = move.from.rank + rank_step, file = move.from.file + file_step;
        rank != move.to.rank;
        rank += rank_step, file += file_step) {

        if (game->board[rank][file] != EMPTY) return false;
    }

    return true;
}

static bool is_legal_queen_move(Game* game, Move move) {
    return is_legal_rook_move(game, move) || is_legal_bishop_move(game, move);
}

static bool is_legal_knight_move(Move move) {

    int rank_diff = abs(move.from.rank - move.to.rank);
    int file_diff = abs(move.from.file - move.to.file);

    return ((rank_diff == 2 && file_diff == 1) || (rank_diff == 1 && file_diff == 2));
}

static bool is_legal_king_move(Game* game, Move move) {
    int rank_diff = abs(move.from.rank - move.to.rank);
    int file_diff = abs(move.from.file - move.to.file);

    // Castling
    if (file_diff == 2 && rank_diff == 0) {

        if (!can_current_player_castle(game)) return false;

        int file_step = (move.to.file - move.from.file) / abs(move.from.file - move.to.file);

        for (int file = move.from.file + file_step; file != move.to.file + file_step; file += file_step) {
            if (game->board[move.to.rank][file] != EMPTY) return false; // Check that no pieces block the castling
        }
    }

    // Check if king is castling
    else if (rank_diff > 1 || file_diff > 1) return false; // King moved more than one square

    // King moved, cant castle anymore
    if (game->current_player == WHITE) {
        game->can_white_castle_left = false;
        game->can_white_castle_right = false;
    }
    else {
        game->can_black_castle_left = false;
        game->can_black_castle_right = false;
    }

    return true;
}

static bool is_legal_piece_move(Game* game, Move move) {
    switch (game->board[move.from.rank][move.from.file]) {
        case BLACK_PAWN:
        case WHITE_PAWN:
            return is_legal_pawn_move(game, move);
        case BLACK_ROOK:
        case WHITE_ROOK:
            return is_legal_rook_move(game, move);
        case BLACK_BISHOP:
        case WHITE_BISHOP:
            return is_legal_bishop_move(game, move);
        case BLACK_KNIGHT:
        case WHITE_KNIGHT:
            return is_legal_knight_move(move);
        case BLACK_QUEEN:
        case WHITE_QUEEN:
            return is_legal_queen_move(game, move);
        case BLACK_KING:
        case WHITE_KING:
            return is_legal_king_move(game, move);
    }

    return true;
}

bool is_king_in_check(Game* game, Position* piece_pos) {
    Position current_player_king_position = { 0, 0 };

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (game->board[rank][file] == WHITE_KING && game->current_player == WHITE ||
                game->board[rank][file] == BLACK_KING && game->current_player == BLACK) {
                current_player_king_position.rank = rank;
                current_player_king_position.file = file;
            }
        }
    }

    // Check every enemy piece on the board if it can move into the kings position
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (!is_current_player_piece(game->current_player, game->board[rank][file]) &&
                game->board[rank][file] != EMPTY
            ) {
                Position from_pos = { rank, file };
                Position to_pos = { current_player_king_position.rank, current_player_king_position.file };

                if (is_legal_piece_move(game, (Move) { from_pos, to_pos })) {
                    if (piece_pos != NULL) {
                        piece_pos->rank = from_pos.rank;
                        piece_pos->file = from_pos.file;
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

static bool will_king_be_in_check(Game* game, Move move) {
    Piece piece_to_move = game->board[move.from.rank][move.from.file];
    Piece original_piece = game->board[move.to.rank][move.to.file];

    game->board[move.from.rank][move.from.file] = EMPTY;
    game->board[move.to.rank][move.to.file] = piece_to_move;

    if (is_last_move_en_passant(game, move)) {
        if (game->current_player == WHITE) {
            game->board[move.to.rank + 1][move.to.file] = EMPTY;
        }
        else {
            game->board[move.to.rank - 1][move.to.file] = EMPTY;
        }
    }

    bool will_be_in_check = is_king_in_check(game, NULL);

    game->board[move.from.rank][move.from.file] = piece_to_move;
    game->board[move.to.rank][move.to.file] = original_piece;

    if (is_last_move_en_passant(game, move)) {
        if (game->current_player == WHITE) {
            game->board[move.to.rank + 1][move.to.file] = BLACK_PAWN;
        }
        else {
            game->board[move.to.rank - 1][move.to.file] = WHITE_PAWN;
        }
    }

    return will_be_in_check;
}

bool is_legal_move(Game* game, Move move) {
    if (move.from.file == move.to.file &&
        move.from.rank == move.to.rank) return false; // Moving piece to the same place

    Piece piece_type_to_move = game->board[move.from.rank][move.from.file];
    Piece place_to_move = game->board[move.to.rank][move.to.file];

    if (!is_current_player_piece(game->current_player, piece_type_to_move)) return false; // Moving not his piece

    if (is_current_player_piece(game->current_player, place_to_move)) return false; // Moving to space occupied by own piece

    if (!is_legal_piece_move(game, move)) return false; // Illegal piece specific move

    if (
        (piece_type_to_move == WHITE_KING || piece_type_to_move == BLACK_KING) &&
        abs(move.from.file - move.to.file) == 2
    ) { // If castling king, check for the two files he moving if he will be in check in them

        int file_step = (move.to.file - move.from.file) / abs(move.from.file - move.to.file);

        for (int file = move.from.file + file_step; file != move.to.file + file_step; file += file_step) {
            if (will_king_be_in_check(game, (Move) { { move.from.rank, move.from.file }, { move.from.rank, file } })) return false;
        }
    }
    else if (will_king_be_in_check(game, move)) return false; // Illegal move because of check

    return true;
}

static bool can_pawn_move(Game* game, Position pos) {
    Position possible_positions[4];
    if (game->current_player == WHITE) {
        possible_positions[0] = (Position){ pos.rank + 2, pos.file };
        possible_positions[1] = (Position){ pos.rank + 1, pos.file };
        possible_positions[2] = (Position){ pos.rank + 1, pos.file + 1 };
        possible_positions[3] = (Position){ pos.rank + 1, pos.file - 1 };
    }
    else {
        possible_positions[0] = (Position){ pos.rank - 2, pos.file };
        possible_positions[1] = (Position){ pos.rank - 1, pos.file };
        possible_positions[2] = (Position){ pos.rank - 1, pos.file + 1 };
        possible_positions[3] = (Position){ pos.rank - 1, pos.file - 1 };
    }

    for (int i = 0; i < 4; i++) {
        Position to = possible_positions[i];
        if (!is_legal_position(to)) continue; // Illegal move
        if (is_legal_move(game, (Move) { pos, to })) return true;
    }

    return false;
}

static bool can_knight_move(Game* game, Position pos) {
    Position possible_positions[] = {
        { pos.rank + 2, pos.file - 1 }, { pos.rank + 2, pos.file + 1 },
        { pos.rank + 1, pos.file - 2 }, { pos.rank + 1, pos.file + 2 },
        { pos.rank - 1, pos.file - 2 }, { pos.rank - 1, pos.file + 2 },
        { pos.rank - 2, pos.file - 1 }, { pos.rank - 2, pos.file + 1 },
    };

    for (int i = 0; i < 8; i++) {
        Position to = possible_positions[i];
        if (!is_legal_position(to)) continue; // Illegal move
        if (is_legal_move(game, (Move) { pos, to })) return true;
    }

    return false;
}

static bool can_bishop_move(Game* game, Position pos) {
    int directions[4][2] = {
        { 1, 1 },
        { 1, -1 },
        { -1, 1 },
        { -1, -1 }
    };

    for (int i = 0; i < 4; i++) {
        int rank_step = directions[i][0];
        int file_step = directions[i][1];
        int rank = pos.rank + rank_step;
        int file = pos.file + file_step;

        while (rank >= 0 && rank < 8 && file >= 0 && file < 8) {
            if (is_legal_move(game, (Move) { pos, { rank, file } })) return true;
            rank += rank_step;
            file += file_step;
        }
    }

    return false;
}

static bool can_rook_move(Game* game, Position pos) {

    int directions[4][2] = {
        { 1, 0 },
        { -1, 0 },
        { 0, -1 },
        { 0, 1 },
    };

    for (int i = 0; i < 4; i++) {
        int rank_step = directions[i][0];
        int file_step = directions[i][1];
        int rank = pos.rank + rank_step;
        int file = pos.file + file_step;

        while (rank >= 0 && rank < 8 && file >= 0 && file < 8) {
            if (is_legal_move(game, (Move) { pos, { rank, file } })) return true;
            rank += rank_step;
            file += file_step;
        }
    }

    return false;
}

static bool can_queen_move(Game* game, Position pos) {
    return can_bishop_move(game, pos) || can_rook_move(game, pos);
}

static bool can_king_move(Game* game, Position pos) {
    Position possible_positions[] = {
        { pos.rank - 1, pos.file - 1 }, { pos.rank - 1, pos.file }, { pos.rank - 1, pos.file + 1 },
        { pos.rank,     pos.file - 1 },                             { pos.rank,     pos.file + 1 },
        { pos.rank + 1, pos.file - 1 }, { pos.rank + 1, pos.file }, { pos.rank + 1, pos.file + 1 },
    };

    for (int i = 0; i < 8; i++) {
        Position to = possible_positions[i];
        if (!is_legal_position(to)) continue; // Illegal move
        if (is_legal_move(game, (Move) { pos, to })) return true;
    }

    return false;
}

bool can_piece_move(Game* game, Position pos) {
    switch (game->board[pos.rank][pos.file]) {
        case BLACK_PAWN:
        case WHITE_PAWN:
            return can_pawn_move(game, pos);
        case BLACK_ROOK:
        case WHITE_ROOK:
            return can_rook_move(game, pos);
        case BLACK_BISHOP:
        case WHITE_BISHOP:
            return can_bishop_move(game, pos);
        case BLACK_KNIGHT:
        case WHITE_KNIGHT:
            return can_knight_move(game, pos);
        case BLACK_QUEEN:
        case WHITE_QUEEN:
            return can_queen_move(game, pos);
        case BLACK_KING:
        case WHITE_KING:
            return can_king_move(game, pos);
    }

    return false;
}

static void get_rook_check_path(Game* game, Position king_pos, Position checking_piece_pos, Position* path) {
    
    if (king_pos.rank == checking_piece_pos.rank) {

        int min_file = min(king_pos.file, checking_piece_pos.file);
        int max_file = max(king_pos.file, checking_piece_pos.file);

        for (int i = 0, file = min_file + 1; file < max_file; file++, i++) {
            path[i] = (Position){ king_pos.rank, file };
        }
    }
    else {
        int min_rank = min(king_pos.rank, checking_piece_pos.rank);
        int max_rank = max(king_pos.rank, checking_piece_pos.rank);

        for (int i = 0, rank = min_rank + 1; rank < max_rank; rank++, i++) {
            path[i].rank = rank;
            path[i].file = king_pos.file;
        }
    }
}

static void get_bishop_check_path(Game* game, Position king_pos, Position checking_piece_pos, Position* path) {
    int rank_diff = abs(king_pos.rank - checking_piece_pos.rank);
    int file_diff = abs(king_pos.file - checking_piece_pos.file);

    int rank_step = (king_pos.rank - checking_piece_pos.rank) / rank_diff;
    int file_step = (king_pos.file - checking_piece_pos.file) / file_diff;

    for (int rank = checking_piece_pos.rank + rank_step,
             file = checking_piece_pos.file + file_step,
             i = 0;
        rank != king_pos.rank;
        rank += rank_step, file += file_step, i++) {
    

        path[i].rank = rank;
        path[i].file = file;
    
        //printf("\nget_bishop_check_path: %d %d\n", path[i].rank, path[i].file);

    }
}

static void get_queen_check_path(Game* game, Position king_pos, Position checking_piece_pos, Position* path) {
    if (checking_piece_pos.rank == king_pos.rank || checking_piece_pos.file == king_pos.file)
        get_rook_check_path(game, king_pos, checking_piece_pos, path);
    else
        get_bishop_check_path(game, king_pos, checking_piece_pos, path);
}

void get_piece_check_path(Game* game, Position king_pos, Position checking_piece_pos, Position* path) {

    Piece piece_type = game->board[checking_piece_pos.rank][checking_piece_pos.file];

    // Can only block rooks, bishops, and queens
    switch (piece_type) {
        case BLACK_ROOK:
        case WHITE_ROOK:
            get_rook_check_path(game, king_pos, checking_piece_pos, path);
            break;
        case BLACK_BISHOP:
        case WHITE_BISHOP:
            get_bishop_check_path(game, king_pos, checking_piece_pos, path);
            break;
        case BLACK_QUEEN:
        case WHITE_QUEEN:
            get_queen_check_path(game, king_pos, checking_piece_pos, path);
    }
}
