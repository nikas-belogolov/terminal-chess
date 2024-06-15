
#include "board.h"
#include "game.h"
#include "io.h"

#define THIN_SPACE u8" "


const char* chess_pieces[NUM_CHESS_PIECES] = {
    u8" ",
    u8"♚",
    u8"♛",
    u8"♜",
    u8"♝",
    u8"♞",
    u8"♟︎",
    u8"♔",
    u8"♕",
    u8"♖",
    u8"♗",
    u8"♘",
    u8"♙"
};

void board_init(Piece board[8][8]) {
    board[0][0] = board[0][7] = WHITE_ROOK;
    board[0][1] = board[0][6] = WHITE_KNIGHT;
    board[0][2] = board[0][5] = WHITE_BISHOP;
    board[0][3] = WHITE_QUEEN;
    board[0][4] = WHITE_KING;

    board[7][0] = board[7][7] = BLACK_ROOK;
    board[7][1] = board[7][6] = BLACK_KNIGHT;
    board[7][2] = board[7][5] = BLACK_BISHOP;
    board[7][3] = BLACK_QUEEN;
    board[7][4] = BLACK_KING;

    for (int i = 0; i < 8; i++) {
        board[1][i] = WHITE_PAWN;
        board[6][i] = BLACK_PAWN;
    }

    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = EMPTY;
        }
    }
}

bool is_last_move_en_passant(Game* game, Move move) {
    if (game->board[move.from.rank][move.from.file] == WHITE_PAWN ||
        game->board[move.from.rank][move.from.file] == BLACK_PAWN) { // Moving pawn
        if (abs(move.to.file - move.from.file) == 1 &&
            game->board[move.to.rank][move.to.file] == EMPTY) return true; // Moved diagonally without capturing piece
    }

    return false;
}

bool is_pawn_promotion(Game* game, Move move) {
    if (game->board[move.from.rank][move.from.file] != BLACK_PAWN &&
        game->board[move.from.rank][move.from.file] != WHITE_PAWN) return false;

    int pawn_promotion_rank = game->current_player == WHITE ? 7 : 0;

    return move.to.rank == pawn_promotion_rank;
}

void board_move(Game* game, Move move) {
    Piece piece_to_move = game->board[move.from.rank][move.from.file];

    if (piece_to_move != WHITE_PAWN &&
        piece_to_move != BLACK_PAWN &&
        game->board[move.to.rank][move.to.file] == EMPTY) game->turns_until_draw--;
    else game->turns_until_draw = 100;

    if (is_last_move_en_passant(game, move)) {
        if (game->current_player == WHITE) {
            game->board[move.to.rank + 1][move.to.file] = EMPTY;
        }
        else {
            game->board[move.to.rank - 1][move.to.file] = EMPTY;
        }
    }
    
    game->board[move.to.rank][move.to.file] = is_pawn_promotion(game, move) ? get_pawn_promotion(game) : piece_to_move;
    game->board[move.from.rank][move.from.file] = EMPTY;

    if ((piece_to_move == WHITE_KING || piece_to_move == BLACK_KING) &&
         abs(move.from.file - move.to.file) == 2) { // Castling, move rook also

        if (move.from.file - move.to.file > 0) { // Castling to the left
            game->board[move.from.rank][0] = EMPTY;
            game->board[move.from.rank][3] = game->current_player == WHITE ? WHITE_ROOK : BLACK_ROOK;

        }
        else {
            game->board[move.from.rank][7] = EMPTY;
            game->board[move.from.rank][5] = game->current_player == WHITE ? WHITE_ROOK : BLACK_ROOK;
        }
    }

    game->current_player = !game->current_player; // change players
    game->last_move = move; // save last move for en passant
}

void board_print(Game* game) {
    if (game->current_player == WHITE) {
        for (char i = 'A'; i <= 'H'; i++) {
            printf("  %c ", i);
        }
    }
    else {
        for (char i = 'H'; i >= 'A'; i--) {
            printf("  %c ", i);
        }
    }

    printf("\n---------------------------------\n");

    if (game->current_player == WHITE) {
        char row_num = '8';

        for (int i = 7; i >= 0; i--) {
            printf("|");

            for (int j = 0; j <= 7; j++) {
                if (game->board[i][j] == WHITE_PAWN) {
                    printf("%s%s", THIN_SPACE, chess_pieces[game->board[i][j]]);
                }
                else {
                    printf(" %s ", chess_pieces[game->board[i][j]]);
                }
                printf("|");

            }
            printf(" %c\n", row_num--);
            printf("---------------------------------\n");
        }
    }
    else {
        char row_num = '1';

        for (int i = 0; i < 8; i++) {
            printf("|");

            for (int j = 7; j >= 0; j--) {
                if (game->board[i][j] == WHITE_PAWN) {
                    printf("%s%s", THIN_SPACE, chess_pieces[game->board[i][j]]);
                }
                else {
                    printf(" %s ", chess_pieces[game->board[i][j]]);

                }
                printf("|");

            }
            printf(" %c\n", row_num++);
            printf("---------------------------------\n");
        }
    }
}