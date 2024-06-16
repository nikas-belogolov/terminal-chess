
#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <stdbool.h>

typedef enum {
	WHITE,
	BLACK
} Player;

typedef struct Game {
    Piece board[8][8];
    Player current_player;
    Move last_move;
    int turns_until_draw;
    bool is_game_over;

    // Castling rights
    bool can_white_castle_right;
    bool can_white_castle_left;
    bool can_black_castle_right;
    bool can_black_castle_left;
} Game;

Game* game_create(void);
bool game_is_over(Game* game);
void game_print_winner(Game* game);

#endif