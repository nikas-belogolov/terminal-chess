

#ifndef UTILS_H
#define UTILS_H

#include "board.h"
#include "game.h"
#include <stdbool.h>

bool is_white_piece(Piece piece);
bool is_black_piece(Piece piece);
bool can_current_player_castle(Game* game);
bool is_current_player_piece(Player current_player, Piece piece);
bool is_legal_position(Position pos);

#endif