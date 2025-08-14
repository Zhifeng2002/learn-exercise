#include <stdio.h>
#include <stdlib.h>

#define MINE_MARK 0xFF

struct field_desc{
	struct field_desc *next;
	int m;
	int n;
	char *data;
	int *tip;
};


struct field_desc_header {
	int field_num;
	struct field_desc *first;
	struct field_desc *last;
} header = {0};

void calculate_one_grid_tip(struct field_desc *field, int i, int j)
{
	int n = field->n;
	int m = field->m;
	int k = i * n + j; 
	if (field->data[k])
		field->tip[k] = MINE_MARK;
	else {
		int tip = 0;
		// above row
		if (i > 0) {
			if (j > 0)
				tip += field->data[(i-1)*n+(j-1)];
			tip += field->data[(i-1)*n+j];
			if (j < n-1)
				tip += field->data[(i-1)*n+(j+1)];
		}

		// current row
		if (j > 0)
			tip += field->data[i*n+(j-1)];
		if (j < n-1)
			tip += field->data[i*n+(j+1)];

		// below row
		if (i < m-1) {
			if (j > 0)
				tip += field->data[(i+1)*n+(j-1)];
			tip += field->data[(i+1)*n+j];
			if (j < n-1)
				tip += field->data[(i+1)*n+(j+1)];
		}

		field->tip[k] = tip;
	}
		
}

void calculate_one_field_tips(struct field_desc *field)
{
	int i , j;
	for (i = 0; i < field->m; ++i)
		for (j = 0; j < field->n; ++j)
			calculate_one_grid_tip(field, i, j);
}

void calculate_all_field_tips() 
{
	struct field_desc * field = header.first;
	while (field) {
		calculate_one_field_tips(field);
		field = field->next;
	}
}

void output_field_tips()
{
	int seq = 1;
	int i, j;

	struct field_desc * field = header.first;
	while (field) {
		if (seq > 1)
			printf("\n");
		printf("Field #%d:\n", seq);

		for (i = 0; i < field->m; ++i) {
			for (j = 0; j < field->n; ++j) {
				int tip = field->tip[i * field->n + j];
				if (tip == MINE_MARK)
					putchar('*');
				else 
					printf("%d", tip);
			}
			printf("\n");
		}
		field = field->next;
		seq++;
	}
}

int main(void)
{
	int m, n;
	int i, j;
	int ret;
	struct field_desc *temp;

	do {
		scanf("%d %d", &m, &n);
		if ( !m || !n)
			break;
		temp = malloc(sizeof(*temp));
		temp->m = m;
		temp->n = n;
		temp->data = malloc(sizeof(char) * m * n);
		temp->tip = malloc(sizeof(int) * m * n);

		// consume line-feed character
		scanf("\n");

		for(i = 0; i < m; ++i) {
			for (j = 0; j < n; ++j) {
				char ch = (char) getchar();
				temp->data[i * n + j] = (ch == '*') ? 1 : 0;
			}
			scanf("\n");
		}

		if (!header.field_num) 
			header.first = header.last = temp;
		else 
		{
			header.last->next = temp;
			header.last = temp;
		}
		header.field_num++;
	} while(1);


	calculate_all_field_tips();
	output_field_tips();

	return 0;
}
