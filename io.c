#include "io.h"
#include "game.h"

#include <stdbool.h>

#define RED_COLOR_START "\033[0;31m"
#define RED_COLOR_END "\033[0m"

void clear_input_buffer() {
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
}

void ask_coordinates(char q[], char* cord) {
	do {
		printf("%s ", q);
		fgets(cord, 3, stdin);
		clear_input_buffer();

		if (cord[0] >= 'a' && cord[0] <= 'h' && cord[1] >= '1' && cord[1] <= '8') break;

		printf("%sPlease enter correct coordinates!%s\n", RED_COLOR_START, RED_COLOR_END);

	} while (true);
}

Piece get_pawn_promotion() {

}

Move get_move(Game* game) {
	char from[3];
	char to[3];
	from[2] = to[2] = '\0';

	Position from_pos, to_pos;
	Move move;

	do {

		ask_coordinates("What piece do you want to move?", from);

		ask_coordinates("Where do you want to move it?", to);

		from_pos = (Position){ from[1] - '0' - 1, toupper(from[0]) - 'A' };
		to_pos = (Position){ to[1] - '0' - 1, toupper(to[0]) - 'A' };
		move = (Move){ from_pos, to_pos };

		if (is_legal_move(&move)) break;
		clear_screen();
		board_print(game);
		printf("\nPlayer to move: %s\n", game->current_player == WHITE ? "white" : "black");

		printf("%sIllegal move, enter other move.%s\n", RED_COLOR_START, RED_COLOR_END);

	} while (true);

	return move;
}