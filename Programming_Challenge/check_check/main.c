#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_CHECK 0
#define BLACK_IN_CHECK 1
#define WHITE_IN_CHECK 2
#define BLACK_PIECE 1
#define WHITE_PIECE 2

char board[8][10];  // add 2 extra char to each line to accommondate '\n' and '\0'


int board_count = 0;
int *results;
int result_len = 10;

struct position{
	int i;
	int j;
};

bool valid_pos(int i, int j) {
	return i >= 0 && i < 8 && j >= 0 && j < 8;
}

bool is_empty_board() 
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (board[i][j] != '.') 
				return false;
	return true;
}

bool read_board() 
{
	int i, j;
	for (i = 0; i < 8; i++) {
		if (!fgets(board[i], sizeof(board[i]), stdin)) {
			return false;
		}

		int len = strlen(board[i]);
		if (len > 0 && board[i][len-1] == '\n') {
			board[i][len-1] = '\0';
		}

		if (strlen(board[i]) != 8) {
			printf("error：length of line #%d is not 8\n", i+1);
			return false;
		}

	}
	return true;
}


int positions_check(struct position *pos, int pos_size, int color) 
{
	int k;
	for (k = 0; k < pos_size; ++k) {
		if (valid_pos(pos[k].i, pos[k].j)){
			if (color == BLACK_PIECE && board[pos[k].i][pos[k].j] == 'K')
				return WHITE_IN_CHECK;
			if (color == WHITE_PIECE && board[pos[k].i][pos[k].j] == 'k')
				return BLACK_IN_CHECK;
		}
	}
	return NO_CHECK;
}


int pawn_check(int i, int j, int color) 
{
	struct position pos[2];
	if (color == BLACK_PIECE) {
		pos[0].i = i+1;
		pos[0].j = j-1;
		pos[1].i = i+1;
		pos[1].j = j+1;
	} else {
		pos[0].i = i-1;
		pos[0].j = j-1;
		pos[1].i = i-1;
		pos[1].j = j+1;
	}
	return positions_check(pos, 2, color);
}

int knight_check(int i, int j, int color) 
{
	// check 8 possible positions
	struct position pos[8] = {
		{i-2, j-1},
		{i-2, j+1},
		{i-1, j-2},
		{i-1, j+2},
		{i+1, j-2},
		{i+1, j+2},
		{i+2, j-1},
		{i+2, j+1},
	};
	return positions_check(pos, 8, color);
}

int rook_check(int i, int j, int color) 
{
	int k;
	struct position pos[4];
	int pos_num = 0;

	// check upward
	for (k = i-1; k >=0; --k) {
		if (board[k][j] != '.') {
			pos[pos_num].i = k;
			pos[pos_num].j = j;
			pos_num++;
			break;
		}
	}

	// check downward
	for (k = i+1; k < 8; ++k) {
		if (board[k][j] != '.') {
			pos[pos_num].i = k;
			pos[pos_num].j = j;
			pos_num++;
			break;
		}
	}

	// check leftward
	for (k = j-1; k >=0; --k) {
		if (board[i][k] != '.') {
			pos[pos_num].i = i;
			pos[pos_num].j = k;
			pos_num++;
			break;
		}
	}

	// check rightward
	for (k = j+1; k < 8; ++k) {
		if (board[i][k] != '.') {
			pos[pos_num].i = i;
			pos[pos_num].j = k;
			pos_num++;
			break;
		}
	}

	return positions_check(pos, pos_num, color);
}

int bishop_check(int i, int j, int color) 
{
	int k;
	struct position pos[4];
	int pos_num = 0;

	// check up-leftward
	for (k = 1; k < 8; ++k) {
		int i_ = i - k;
		int j_ = j - k;
		if (!valid_pos(i_, j_))
			break;
		if (board[i_][j_] != '.') {
			pos[pos_num].i = i_;
			pos[pos_num].j = j_;
			pos_num++;
			break;
		}
	}

	// check up-rightward
	for (k = 1; k < 8; ++k) {
		int i_ = i - k;
		int j_ = j + k;
		if (!valid_pos(i_, j_))
			break;
		if (board[i_][j_] != '.') {
			pos[pos_num].i = i_;
			pos[pos_num].j = j_;
			pos_num++;
			break;
		}
	}

	// check down-leftward
	for (k = 1; k < 8; ++k) {
		int i_ = i + k;
		int j_ = j - k;
		if (!valid_pos(i_, j_))
			break;
		if (board[i_][j_] != '.') {
			pos[pos_num].i = i_;
			pos[pos_num].j = j_;
			pos_num++;
			break;
		}
	}


	// check down-rightward
	for (k = 1; k < 8; ++k) {
		int i_ = i + k;
		int j_ = j + k;
		if (!valid_pos(i_, j_))
			break;
		if (board[i_][j_] != '.') {
			pos[pos_num].i = i_;
			pos[pos_num].j = j_;
			pos_num++;
			break;
		}
	}

	return positions_check(pos, pos_num, color);
}


int queen_check(int i, int j, int color) 
{
	int result = rook_check(i, j, color);
	if (result == NO_CHECK)
		result = bishop_check(i, j, color);
	return result;
}

int king_check(int i, int j, int color) 
{
	// check 8 possible positions
	struct position pos[8] = {
		{i-1, j-1},
		{i-1, j},
		{i-1, j+1},
		{i, j-1},
		{i, j+1},
		{i+1, j-1},
		{i+1, j},
		{i+1, j+1},
	};
	return positions_check(pos, 8, color);
}


int piece_check(int i, int j) 
{
	switch(board[i][j]) {
		case 'p':
			return pawn_check(i, j, BLACK_PIECE);
		case 'P':
			return pawn_check(i, j, WHITE_PIECE);
		case 'n':
			return knight_check(i, j, BLACK_PIECE);
		case 'N':
			return knight_check(i, j, WHITE_PIECE);
		case 'r':
			return rook_check(i, j, BLACK_PIECE);
		case 'R':
			return rook_check(i, j, WHITE_PIECE);
		case 'b':
			return bishop_check(i, j, BLACK_PIECE);
		case 'B':
			return bishop_check(i, j, WHITE_PIECE);
		case 'q':
			return queen_check(i, j, BLACK_PIECE);
		case 'Q':
			return queen_check(i, j, WHITE_PIECE);
		case 'k':
			return king_check(i, j, BLACK_PIECE);
		case 'K':
			return king_check(i, j, WHITE_PIECE);
		default:
			//should not come here
			break;
	}
	return NO_CHECK;
}


int check_check() 
{
	int i, j;
	int result;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) {
			result = piece_check(i, j);
			if (result != NO_CHECK)
				return result;
		}
	return result;
}

void store_result(int result)
{
	if (board_count > result_len) {
		result_len += 10;
		results = realloc(results, sizeof(*results) * result_len);
	}
	results[board_count - 1] = result;
}

void print_result()
{
	int k;
	for (k = 0; k < board_count; ++k) {
		if (results[k] == WHITE_IN_CHECK)
			printf("Game #%d: white king is in check.\n", k+1);
		else if (results[k] == BLACK_IN_CHECK)
			printf("Game #%d: black king is in check.\n", k+1);
		else
			printf("Game #%d: no king is in check.\n", k+1);
	}
}


int main() 
{
	char line[100];
	results = malloc(sizeof(*results)*result_len);

	while (1) {
		if (!read_board()) {
			printf("error read board\n");
			break;
		}

		if (is_empty_board()) {
			break;
		}

		board_count++;
		store_result(check_check());

		if (fgets(line, sizeof(line), stdin)) {
			if (strlen(line) > 1) {
				printf("warning：expect blank line while get: %s", line);
				break;
			}
		}
	}

	if (board_count) {
		print_result();
	}

	free(results);

	return 0;
}
