#include <stdio.h>
#include <stdlib.h>

// max display array  
#define MAX_LCD_HEIGHT (2*10+3)
#define MAX_LCD_WIDTH (12*8+9)

char display[MAX_LCD_HEIGHT][MAX_LCD_WIDTH];

char digits[8];
char dig_len;

enum place {
	TOP,
	MID,
	BOT,
	LEFT_TOP,
	RIGHT_TOP,
	LEFT_BOT,
	RIGHT_BOT,
};

void create_digits(int n)
{
	dig_len = 0;
	while (n) {
		digits[dig_len] = n % 10;
		n /=10;
		dig_len++;
	}

	// handle special case that n == 0
	if (!dig_len) {
		digits[0] = 0;
		dig_len = 1;
	}
}

void draw_horizon(int s, int pos, enum place pl)
{
	int row_offset;
	int col_offset = 1;
	int i;

	switch (pl) {
		case TOP:
			row_offset = 0;
			break;
		case MID:
			row_offset = s + 1;
			break;
		case BOT:
			row_offset = s * 2 + 2;
			break;
		default:
			fprintf(stderr, "incorrect place specified.\n");
			exit(-1);
	}

	for (i = 0; i < s; ++i) 
		display[row_offset][pos + col_offset + i] = '-';
}

void draw_vert(int s, int pos, enum place pl)
{
	int row_offset;
	int col_offset;
	int i;

	switch (pl) {
		case LEFT_TOP:
			row_offset = 1;
			col_offset = 0;
			break;
		case LEFT_BOT:
			row_offset = s + 2;
			col_offset = 0;
			break;
		case RIGHT_TOP:
			row_offset = 1;
			col_offset = s + 1;
			break;
		case RIGHT_BOT:
			row_offset = s + 2;
			col_offset = s + 1;
			break;
		default:
			fprintf(stderr, "incorrect place specified.\n");
			exit(-1);
	}

	for (i = 0; i < s; ++i) 
		display[row_offset + i][pos + col_offset] = '|';
}


void draw_dig(int s, char dig, int pos)
{
	switch (dig) {
		case 1:
			draw_vert(s, pos, RIGHT_TOP);
			draw_vert(s, pos, RIGHT_BOT);
			break;
		case 2:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, LEFT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		case 3:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, RIGHT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		case 4:
			draw_vert(s, pos, LEFT_TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, RIGHT_BOT);
			break;
		case 5:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, LEFT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, RIGHT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		case 6:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, LEFT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, LEFT_BOT);
			draw_vert(s, pos, RIGHT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		case 7:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_vert(s, pos, RIGHT_BOT);
			break;
		case 8:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, LEFT_TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, RIGHT_BOT);
			draw_vert(s, pos, LEFT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		case 9:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, LEFT_TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_horizon(s, pos, MID);
			draw_vert(s, pos, RIGHT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		case 0:
			draw_horizon(s, pos, TOP);
			draw_vert(s, pos, LEFT_TOP);
			draw_vert(s, pos, RIGHT_TOP);
			draw_vert(s, pos, RIGHT_BOT);
			draw_vert(s, pos, LEFT_BOT);
			draw_horizon(s, pos, BOT);
			break;
		default:
			break;
	}
}

void create_display_content(int s, int n, int *width)
{
	int pos = 0;
	int i, j;

	// reset display to blank
	for (i = 0; i < MAX_LCD_HEIGHT; ++i)
	       for (j = 0; j < MAX_LCD_WIDTH; ++j)
		       display[i][j] = ' ';

	create_digits(n);

	while(dig_len) {
		char dig = digits[dig_len - 1];
		draw_dig(s, dig, pos);
		pos += s + 2 + 1; // add a blank column between digits
		dig_len--;
	}
	*width = pos - 1;
}

void show_display_content(int s, int width)
{
	int i;
	int rows = 2 * s + 3;
	for (i = 0; i < rows; ++i) {
		display[i][width] = '\0';
		puts(display[i]);
	}
}

int main(int argc, char **argv)
{
	int s, n;
	int width;
	FILE *input;
	int num = 0;
	
	if (argc < 2) {
		printf("usage:\n");
		printf("%s <input.txt>\n", argv[0]);
		exit(-1);
	}

	input = fopen(argv[1], "r");
	if (!input) {
		fprintf(stderr, "Failed to open input file %s\n", argv[1]);
		exit(-1);
	}

	while (1) {
		int ret = fscanf(input, "%d %d", &s, &n);
		if (!s || ret == EOF)
			break;
		if (s < 0 || s > 10 || n < 0 || n > 99999999) {
			fprintf(stderr, "invalid input\n");
			exit(-1);
		}
		++num;
		create_display_content(s, n, &width);

		// add a blank line for every number from 2nd
		if (num > 1)
			printf("\n");
		show_display_content(s, width);

	}

	return 0;
}
