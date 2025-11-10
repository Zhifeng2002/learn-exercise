#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define WORD_MAX_LEN 17
#define DICT_MAX_ELE 1000
#define LINE_SIZE 81
#define MAX_INPUT 40
#define NULL_STEP 1000

#define TO_MAP_INDEX(ch) (ch-'a')


int dict_size;
char dict[DICT_MAX_ELE][WORD_MAX_LEN];
char encrypt[MAX_INPUT][WORD_MAX_LEN];
int input_cnt;

bool complete_success;
struct char_map {
	char decrypt;
	int step;
} map[26];


void prepare()
{
	int i;
	input_cnt = 0;
	for (i = 0; i < 26; ++i) 
		map[i].step = NULL_STEP;
	complete_success = false;
}


bool apply_char_map(char *encrypt, char *dict_item, int step)
{
	int i, j;
	int len = strlen(encrypt);
	struct char_map *cm;

	for (i = 0; i < len; ++i) {
		cm = &map[TO_MAP_INDEX(encrypt[i])];
		if (cm->step <= step && cm->decrypt != dict_item[i])
			return false;

		// reverse check if the decrypt is already mapped
		// we don't want 2 encrypts for 1 char
		for (j = 0; j < 26; ++j) 
			if (map[j].step <= step 
					&& map[j].decrypt == dict_item[i]
					&& j != TO_MAP_INDEX(encrypt[i]))
				return false;

		if (cm->step > step) {
			cm->step = step;
			cm->decrypt = dict_item[i];
		}
	}
		
	return true;
}

void revert_map(int step)
{ 
	int i = 0; 
	for (i = 0; i < 26; ++i)
		if (map[i].step >= step) map[i].step = NULL_STEP; 
} 

bool decrypt(int step) 
{
	int i;
	if (step == input_cnt) {
		complete_success = true;
		return true;
	}

	// go through dictionary to find possible decryption
	for (i = 0; i < dict_size; ++i) {
		if (strlen(encrypt[step]) != strlen(dict[i]))
			continue;
		if (apply_char_map(encrypt[step], dict[i], step))
			if (decrypt(step+1))
				return true;
		revert_map(step);
	}

	return false;
}


void decrypt_and_print(char *encrypt)
{
	int len = strlen(encrypt);
	int i;
	for (i = 0; i < len; ++i) {
		if (encrypt[i] == ' ')
			continue;
		if (complete_success) 
			encrypt[i] = map[TO_MAP_INDEX(encrypt[i])].decrypt;
		else
			encrypt[i] = '*';
	}
	puts(encrypt);
}

int main() {
	char line[LINE_SIZE];
	char line_bak[LINE_SIZE];
	scanf("%d", &dict_size);
	for (int i = 0; i < dict_size; i++) {
		scanf("%s", dict[i]);
	}
	getchar();

	while (fgets(line, LINE_SIZE, stdin)) {
		line[strcspn(line, "\n")] = '\0';
		strcpy(line_bak, line);
		prepare();
		char *token = strtok(line, " \t\n\r");
		while (token != NULL) {
			strcpy(encrypt[input_cnt++], token);
			token = strtok(NULL, " \t\n\r.,!?;:");
		}

		if (input_cnt) 
			decrypt(0);

		decrypt_and_print(line_bak);
	}

	return 0;
}
