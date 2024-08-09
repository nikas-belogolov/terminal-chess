#include "io.h"
#include "game.h"
#include "move.h"
#include <ctype.h>
#include <stdbool.h>

#define RED_COLOR_START "\033[0;31m"
#define RED_COLOR_END "\033[0m"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void clear_screen(void)
{
#ifdef _WIN32
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR)' ',
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
#else
const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
   write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 11);
#endif
}


static void clear_input_buffer(void) {
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
}

static void ask_coordinates(char q[], char* cord) {
	do {
		printf("%s ", q);
		fgets(cord, 3, stdin);
		clear_input_buffer();

		if (cord[0] >= 'a' && cord[0] <= 'h' && cord[1] >= '1' && cord[1] <= '8') break;

		printf("%sPlease enter correct coordinates!%s\n", RED_COLOR_START, RED_COLOR_END);

	} while (true);
}

Piece get_pawn_promotion(Game* game) {
	printf("Choose piece to promote your pawn to:\n");
	printf("Queen (Q) | Rook (R) | Bishop (B) | Knight (K)\n");

	char piece[2];
	piece[1] = '\0';

	do {
		fgets(piece, 2, stdin);
		clear_input_buffer();

		piece[0] == (char)tolower((unsigned char)piece[0]);

		if (piece[0] == 'q' || piece[0] == 'r' || piece[0] == 'b' || piece[0] == 'k') break;

		printf("%sPlease choose a queen, rook, bishop, or knight as a promotion piece!%s\n", RED_COLOR_START, RED_COLOR_END);


	} while (true);

	switch (piece[0]) {
		case 'q':
			return game->current_player == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
		case 'r':
			return game->current_player == WHITE ? WHITE_ROOK : BLACK_ROOK;
		case 'b':
			return game->current_player == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
		case 'k':
			return game->current_player == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
	}
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

		if (is_legal_move(game, move)) break;
		/*clear_screen();
		board_print(game);*/
		printf("\nPlayer to move: %s\n", game->current_player == WHITE ? "white" : "black");

		printf("%sIllegal move, enter other move.%s\n", RED_COLOR_START, RED_COLOR_END);

	} while (true);

	return move;
}