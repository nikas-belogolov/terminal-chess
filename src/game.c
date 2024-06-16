


#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "utils.h"
#include "move.h"

Game* game_create(void) {
	Game* game = (Game*)malloc(sizeof(Game));

	if (game == NULL) return NULL;

	game->is_game_over = false;
    game->last_move = (Move){ {-1, -1}, {-1, -1} };
	game->turns_until_draw = 100;
	game->current_player = WHITE;

	game->can_black_castle_left = true;
	game->can_black_castle_right = true;
	game->can_white_castle_left = true;
	game->can_white_castle_right = true;

	board_init(game->board);

	return game;
}

static bool is_checkmate(Game* game) {
    Position checking_piece_pos = { 0, 0 };
    if (!is_king_in_check(game, &checking_piece_pos)) return false;


    // Check if king can move in any direction without getting a check, or if one of current player pieces can capture the checking piece
    int king_rank;
    int king_file;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (game->board[rank][file] == WHITE_KING && game->current_player == WHITE ||
                game->board[rank][file] == BLACK_KING && game->current_player == BLACK) {
                king_rank = rank;
                king_file = file;
            }
        }
    }

    Position king_pos = { king_rank, king_file };

    Position possible_positions[8] = {
        { king_rank - 1, king_file - 1 }, { king_rank - 1, king_file }, { king_rank - 1, king_file + 1 },
        { king_rank,     king_file - 1 },                               { king_rank,     king_file + 1 },
        { king_rank + 1, king_file - 1 }, { king_rank + 1, king_file }, { king_rank + 1, king_file + 1 },

    };

    // Loop over all king possible moves and check if he can escape a check
    for (int i = 0; i < 8; i++) {
        Position pos = possible_positions[i]; // Possible king move to escape check

        if (!is_legal_position(pos)) continue; // Invalid move
        Position from = { king_rank, king_file };
        Position to = { pos.rank, pos.file };

        if (is_legal_move(game, (Move) { from, to })) return false;
    }



    // Check if any piece can eat the checking piece
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (is_current_player_piece(game->current_player, game->board[rank][file])) {
                // Check if  piece can eat the checking piece
                if (is_legal_move(game, (Move) { { rank, file }, { checking_piece_pos.rank, checking_piece_pos.file } })) {
                    printf("\npiece type: %d rank: %d file: %d\n", game->board[rank][file], rank, file);
                    return false;

                }

                Position checking_path[7] = { -1, -1 };

                // Get all the squares between king and checking piece
                get_piece_check_path(game, king_pos, checking_piece_pos, checking_path);

                int i = 0;
                // Check if piece can block the check path
                
                for (int i = 0; i < 7; i++) {
                    if (checking_path[i].rank == -1) break;
                    if (is_legal_move(game, (Move) { { rank, file }, checking_path[i] })) {
                        printf("Not Checkmate!!!!!");
                        return false;

                    }
                }
            }
        }
    }

    printf("Checkmate!!!!!");

    return true;
}

static bool is_stalemate(Game* game) {
	if (game->turns_until_draw == 0) return true;

	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			if (
				is_current_player_piece(game->current_player, game->board[rank][file]) &&
				can_piece_move(game, (Position){ rank, file })
				) return false;
		}
	}

	return true;
}

bool game_is_over(Game* game) {
	return is_checkmate(game) || is_stalemate(game);
}

void game_print_winner(Game* game) {
    if (is_checkmate(game)) {
        printf("%s player won!", game->current_player == WHITE ? "Black" : "White");
        return;
    }

	printf("Stalemate, no one won!");
}