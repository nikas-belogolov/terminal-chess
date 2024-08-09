#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "board.h"


void clear_screen(void);

Piece get_pawn_promotion(Game* game);
Move get_move(Game* game);

#endif