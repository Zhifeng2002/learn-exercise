#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TO_INDEX(x, y) ((x-1)+(y-1)*m)

static char * graph = NULL;
int m, n;

void create_graph(int m, int n)
{
	if(graph)
		free(graph);
	graph = calloc(m*n, 1);
}

void clear_graph()
{
	if (graph)
		memset(graph, 0, m*n); 
}

void color_pixel(int x, int y, char color)
{
	//Note: ignore x y validation, assume always valid
	if (graph)
		graph[TO_INDEX(x,y)] = color;
}

void color_rect(int x1, int x2, int y1, int y2, char color)
{
	int x, y; 

	if (!graph)
		return;

	for (x = x1; x <= x2; x++)
		for (y = y1; y <= y2; y++)
			color_pixel(x, y, color);
}

void calc_region(char *region, int x, int y, char color)
{
	if (region[TO_INDEX(x, y)])
		return; // already in region
	
	region[TO_INDEX(x, y)] = 1;

	// check surrounding 4 pixels 
	// left 
	if (x > 1 && graph[TO_INDEX(x-1, y)] == color)
		calc_region(region, x-1, y, color);

	// right
	if (x < m && graph[TO_INDEX(x+1, y)] == color)
		calc_region(region, x+1, y, color);

	// top
	if (y>1 && graph[TO_INDEX(x, y-1)] == color)
		calc_region(region, x, y-1, color);

	//bottom
	if (y<n && graph[TO_INDEX(x, y+1)] == color)
		calc_region(region, x, y+1, color);
}

void fill_region(int x, int y, char color)
{
	int x1, y1;
	char *region = calloc(m*n, 1);
	calc_region(region, x, y, graph[TO_INDEX(x, y)]); 
	for (y1 = 1; y1 <=n; ++y1)
		for (x1 =  1; x1 <= m; ++x1) 
			if (region[TO_INDEX(x1, y1)])
				color_pixel(x1, y1, color);
	free(region);
}

void print_to_file(FILE *file)
{
	int x, y;
	char c;
	for (y = 1; y <=n; ++y) {
		for (x = 1; x <= m; ++x) {
			c = graph[TO_INDEX(x, y)];
			if (!c)
				c = '0';
			fputc(c, file);
		}
		fputc('\n', file);
	}
}

void save_graph(char *filename)
{
	FILE *file = fopen(filename, "w+");
	print_to_file(file);
	fclose(file);
}



int main() {
    char command;
    int x1, x2, y1, y2;
    char color;
    int result;
    char filename[256];

    while (1) {
        printf("Enter command and parameters (e.g., I 5 6 or X to quit): ");
        result = scanf(" %c", &command);
        if (result == EOF) {
            break; 
        }

        switch (command) {
            case 'I':
                scanf("%d %d", &m, &n);
		create_graph(m, n);
                break;
            case 'C':
		clear_graph();
                break;
            case 'L':
                scanf("%d %d %c", &x1, &y1, &color);
		color_pixel(x1, y1, color);
                break;
            case 'V':
                scanf("%d %d %d %c", &x1, &y1, &y2, &color);
		color_rect(x1, x1, y1, y2, color);
                break;
            case 'H':
                scanf("%d %d %d %c", &x1, &x2, &y1, &color);
		color_rect(x1, x2, y1, y1, color);
                break;
            case 'K':
                scanf("%d %d %d %d %c", &x1, &y1, &x2, &y2, &color);
		color_rect(x1, x2, y1, y2, color);
                break;
            case 'F':
                scanf("%d %d %c", &x1, &y1, &color);
		fill_region(x1, y1, color);
                break;
            case 'S':
                scanf("%255s", filename);
		save_graph(filename);
		break;
            case 'P':
		print_to_file(stdout);
		break; 
            case 'X':
                return 0;
            default:
                printf("Unknown command: %c\n", command);
                break;
        }
        while (getchar() != '\n');
    }

    return 0;
}
