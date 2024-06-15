#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>
#include "game.h"
#include "board.h"

bool can_piece_move(Game* game, Position pos);
bool is_legal_move(Game* game, Move move);
bool is_king_in_check(Game* game);

#endif
