#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 100
#define MAX_CARDS 10
#define MAX_RESULTS 1000

struct card {
    char value;  // A, 2-9, T, J, Q, K
    char suit;  // H, D, S, C
};

struct card cards[MAX_CARDS];
int count;

enum rank_type {
	HIGH_CARD = 1,
	PAIR,
	TWO_PAIR,
	THREE_OF_KIND,
	STRAIGHT,
	FLUSH,
	FULL_HOUSE,
	FOUR_OF_KIND,
	STRAIGHT_FLUSH
};


struct rank_info {
	enum rank_type rank_type;
	struct card high_card[5];
	int num_high_card;
	int type_val[2];
	int num_type;
} white_rank, black_rank;

// store results in array
enum rank_result {
	BLACK_WIN,
	WHITE_WIN,
	DRAW
} rank_results[MAX_RESULTS];
int result_count = 0;

bool parse_card(char* token, struct card *c)
{
	if (strlen(token) != 2)
		goto error;

	switch (token[0]) {
		case 'T':
			c->value = 10;
			break;
		case 'J':
			c->value = 11;
			break;
		case 'Q':
			c->value = 12;
			break;
		case 'K':
			c->value = 13;
			break;
		case 'A':
			c->value = 14;
			break;
		default:
			if (token[0] >= '2' && token[0] <= '9')
				c->value = token[0] - '0';
			else 
				goto error;
			break;
	};

	switch (token[1]) {
		case 'H':
		case 'D':
		case 'S':
		case 'C':
			c->suit = token[1];
			break;
		default:
			goto error;
	};
	return true;

error:
	printf("detected incorrect card: %s\n", token);
	return false;
}

static int card_descend_compare(const void *s1, const void *s2)
{
	const struct card *c1 = s1;
	const struct card *c2 = s2;

	if (c1->value < c2->value)
		return 1;
	else if (c1->value == c2->value)
		return 0;
	else
		return -1;
}

void calc_rank_info(struct rank_info *rank_info, struct card *cards)
{
	bool straight = true;
	bool flush = true;
	int i;
	int state;

	memset(rank_info, 0, sizeof(*rank_info));
	qsort(cards, 5, sizeof(*cards), card_descend_compare);

	// first check if hand is flush or straight to reduce possible states
	for (i = 0; i < 4; i++) {
		if (cards[i].value != cards[i+1].value+1)
			straight = false;
		if (cards[i].suit != cards[i+1].suit)
			flush = false;
	}

	if (flush || straight) {
		memcpy(rank_info->high_card, cards, sizeof(rank_info->high_card));
		rank_info->num_high_card = 5;
		if (straight && flush)
			rank_info->rank_type = STRAIGHT_FLUSH;
		else if(straight)
			rank_info->rank_type = STRAIGHT;
		else
			rank_info->rank_type = FLUSH;
		return;
	}


#define ENDSTATE 1000

	// use state machine to detect other rank types
	rank_info->rank_type = HIGH_CARD;
	state = 0;

	while (state != ENDSTATE) {
		if (state == 0) {
			// start state, pattern 'Z????', check second card
			if (cards[1].value == cards[0].value) {
				rank_info->rank_type = PAIR;
				rank_info->type_val[0] = cards[0].value;
				rank_info->num_type = 1;
				state = 1;
			} else {
				rank_info->high_card[0] = cards[0];
				rank_info->num_high_card = 1;
				state = 2;
			}
		}

		if (state == 1) {
			// state 1, pattern 'ZZ???', check 3rd card
			if (cards[2].value == cards[1].value) {
				rank_info->rank_type = THREE_OF_KIND;
				state = 3;
			} else {
				state = 4;
			}

		}

		if (state == 2) {
			// state 2, pattern 'ZY???', check 3rd card
			if (cards[2].value == cards[1].value) {
				rank_info->rank_type = PAIR;
				rank_info->type_val[0] = cards[1].value;
				rank_info->num_type = 1;
				state = 8;
			} else {
				rank_info->high_card[1] = cards[1];
				rank_info->num_high_card = 2;
				state = 9;
			}

		}

		if (state == 3) {
			// state 3, pattern 'ZZZ??', check 4th card
			if (cards[3].value == cards[2].value) {
				rank_info->rank_type = FOUR_OF_KIND;
				state = ENDSTATE;
			} else {
				state = 5;
			}

		}

		if (state == 4) {
			// state 4, pattern 'ZZY??', check 4th card
			if (cards[3].value == cards[2].value) {
				rank_info->rank_type = TWO_PAIR;
				rank_info->type_val[1] = cards[3].value;
				rank_info->num_type = 2;
				state = 6;
			} else {
				rank_info->high_card[0] = cards[2];
				rank_info->num_high_card = 1;
				state = 7;
			}
		}

		if (state == 5) {
			// state 5, pattern 'ZZZY?', check 5th card
			if (cards[4].value == cards[3].value) 
				rank_info->rank_type = FULL_HOUSE;
			state = ENDSTATE;
		}

		if (state == 6) {
			// state 6, pattern 'ZZYY?', check 5th card
			if (cards[4].value == cards[3].value) {
				rank_info->rank_type = FULL_HOUSE;
				rank_info->type_val[0] = cards[4].value;
				rank_info->type_val[1] = cards[1].value;
			} else {
				rank_info->high_card[0] = cards[4];
				rank_info->num_high_card = 1;
			}
			state = ENDSTATE;
		}

		if (state == 7 || state == 10) {
			// state 7, pattern 'ZZYX?', check 5th card
			// state 10, pattern 'ZYYX?', check 5th card
			if (cards[4].value == cards[3].value) {
				rank_info->rank_type = TWO_PAIR;
				rank_info->type_val[1] = cards[4].value;
				rank_info->num_type = 2;
			} else {
				rank_info->high_card[1] = cards[3];
				rank_info->high_card[2] = cards[4];
				rank_info->num_high_card = 3;
			}
			state = ENDSTATE;
		}

		if (state == 8) {
			// state 8, pattern 'ZYY??', check 4th card
			if (cards[3].value == cards[2].value) {
				rank_info->rank_type = THREE_OF_KIND;
				// further check if it is FOUR_OF_KIND to reduce a state 
				if (cards[4].value == cards[3].value)
					rank_info->rank_type = FOUR_OF_KIND;
				state = ENDSTATE;
			} else {
				state = 10;
			}
		}

		if (state == 9) {
			// state 9, pattern 'ZYX??', check 4th card
			if (cards[3].value == cards[2].value) {
				rank_info->rank_type = PAIR;
				rank_info->type_val[0] = cards[3].value;
				rank_info->num_type = 1;
				state = 11;
			} else {
				rank_info->high_card[2] = cards[2];
				rank_info->num_high_card = 3;
				state = 12;
			}
		}

		if (state == 11) {
			// state 11, pattern 'ZYXX?', check 5th card
			if (cards[4].value == cards[3].value) {
				rank_info->rank_type = THREE_OF_KIND;
			} else {
				rank_info->high_card[2] = cards[4];
				rank_info->num_high_card = 3;
			}
			state = ENDSTATE;
		}

		if (state == 12) {
			// state 12, pattern 'ZYXW?', check 5th card
			if (cards[4].value == cards[3].value) {
				rank_info->rank_type = PAIR;
				rank_info->type_val[0] = cards[4].value;
				rank_info->num_type = 1;
			} else {
				rank_info->high_card[3] = cards[3];
				rank_info->high_card[4] = cards[4];
				rank_info->num_high_card = 5;
			}
			state = ENDSTATE;
		}
	}
}

enum rank_result compare_high_card()
{
	int i;
	for (i = 0; i < black_rank.num_high_card; ++i) {
		if (black_rank.high_card[i].value > white_rank.high_card[i].value)
			return BLACK_WIN;
		if (black_rank.high_card[i].value < white_rank.high_card[i].value)
			return WHITE_WIN;
	}
	return DRAW;
}


enum rank_result compare_type_value()
{
	int i;
	for (i = 0; i < black_rank.num_type; ++i) {
		if (black_rank.type_val[i] > white_rank.type_val[i])
			return BLACK_WIN;
		if (black_rank.type_val[i] < white_rank.type_val[i])
			return WHITE_WIN;
	}
	return DRAW;
}



enum rank_result compare_rank()
{
	enum rank_result result;

	if (black_rank.rank_type > white_rank.rank_type)
		return BLACK_WIN;

	if (black_rank.rank_type < white_rank.rank_type)
		return WHITE_WIN;

	result = compare_type_value();
	if (result != DRAW)
		return result;

	return compare_high_card();
}

bool process_cards(char* line) {
    count = 0;
    char* token;
    bool success = true;

    token = strtok(line, " \t\n\r");
    while (token != NULL) {
	   if (count < MAX_CARDS) {
		   if (!parse_card(token, &cards[count])) {
			   success = false;
			   break;
		   }
		   ++count;
	   } else {
		   success = false;
		   printf("too many cards input on line %s\n", line);
		   break;
	   }
        token = strtok(NULL, " \t\n\r");
    }

    if (!success)
	    return false;

    if (count < MAX_CARDS) {
	    printf("Not enough cards, need 10, but only read in %d\n", count);
	    return false;
    }

    calc_rank_info(&black_rank, &cards[0]);
    calc_rank_info(&white_rank, &cards[5]);

    if (result_count < MAX_RESULTS)
	    rank_results[result_count++] = compare_rank();

    return true;
}

int main() 
{
    char line[MAX_LINE_LENGTH];
    int i;

    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '\n' || line[0] == '\0') {
            break;
        }
	if (!process_cards(line))
		break;
    }

    for (i = 0; i < result_count; ++i) {
	    if (rank_results[i] == BLACK_WIN)
		    printf("Black wins.\n");
	    else if (rank_results[i] == WHITE_WIN)
		    printf("White wins.\n");
	    else
		    printf("Tie.\n");
    }
    
    return 0;
}
