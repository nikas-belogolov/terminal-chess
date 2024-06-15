

#ifndef UTILS_H
#define UTILS_H

#include "board.h";
#include <stdbool.h>

bool is_white_piece(ChessPiece piece);
bool is_black_piece(ChessPiece piece);
bool is_current_player_piece(ChessPiece piece);
bool is_legal_position(PiecePosition pos);

#endif