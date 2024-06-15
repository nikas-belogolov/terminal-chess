#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

typedef struct Game Game;

typedef enum {
	EMPTY,
	WHITE_KING,
    WHITE_QUEEN,
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_PAWN,
    NUM_CHESS_PIECES
} Piece;

typedef struct {
    int rank;
    int file;
} Position;

typedef struct {
    Position from;
    Position to;
} Move;

void board_init(Piece board[8][8]);
void board_print(Game* game);
void board_move(Game* game, Move move);

bool is_last_move_en_passant(Game* game, Move move);

#endif