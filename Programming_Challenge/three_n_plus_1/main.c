#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT 1000
struct parameter {
	int i;
	int j;
	int max_cycle;
} paras[MAX_INPUT];


static int input_num = 0;

int find_cycle(int i)
{
	int cycle = 1;
	while (i != 1) {
		if (!(i%2))
			i /= 2;
		else
			i = i * 3 + 1;
		cycle++;

	}
	return cycle;
}

void find_max_cycle(struct parameter *para)
{
	para->max_cycle = 0;
	int m;
	for (m = para->i; m <= para->j; ++m) {
		int cycle = find_cycle(m);
		if (cycle > para->max_cycle)
			para->max_cycle = cycle;
	}
}

int main()
{
	int ret;
	int k;

	do {
		ret = scanf("%d %d", &paras[input_num].i, &paras[input_num].j);
		if (ret == EOF)
			break;
		else if (ret != 2){
			fprintf(stderr, "input error detected\n");
			exit(-1);
		} else 
			input_num++;
	} while (input_num < MAX_INPUT);

	for (k = 0; k < input_num; ++k) {
		find_max_cycle(&paras[k]);
	}

	for (k = 0; k < input_num; ++k) {
		printf("%d %d %d	\n", paras[k].i, paras[k].j, paras[k].max_cycle);
	}

}
