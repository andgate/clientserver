#include "table.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

table_t new_table(int rows, int cols)
{
	if (rows <= 0 || cols <= 0)
	{
		fprintf(stderr, "Table rows and/or columns must be larger than 0.\n");
		exit(EXIT_FAILURE);
	}

	table_t table;
	table.dim.rows = rows;
	table.dim.cols = cols;
	
	// Allocate rows
	table.data = (int**) malloc((rows-1)*sizeof(int*));

	// Allocate columns
	for(int i = 0; i < rows; ++i)
		table.data[i] = (int*) calloc(cols, sizeof(int));

	return table;
}

void delete_table(table_t table)
{
	// Delete columns
	for(int i = 0; i < table.dim.rows; ++i)
		free(table.data[i]);

	// Delete rows
	free(table.data);
}

void display_table(table_t table)
{
	printf("  ");
	for(int i = 0; i < table.dim.cols; ++i)
		printf("%d ", i+1);
	printf("\n");

	for(int i = 0; i < table.dim.rows; ++i)
	{
		printf("%d ", i+1);
		for(int j = 0; j < table.dim.cols; ++j)
		{
			if(!table.data[i][j])
				printf("o ");
			else
				printf("x ");
		}
		printf("\n");
	}
}

void dump_table(table_t table)
{
	printf("%dx%d-Table\n", table.dim.rows, table.dim.cols);

	for(int i = 0; i < table.dim.rows; ++i)
	{
		for(int j = 0; j < table.dim.cols; ++j)
		{
			printf("table[%d][%d] = %d\n", i, j, table.data[i][j]);
		}
	}
}


coords_parse_result table_read_coords(char* str)
{
	coords_parse_result res;
	res.x = -1;
	res.y = -1;
	res.err = COORDS_PARSE_FAILURE;

	char* tok = strtok(str, " ");
	if (tok == NULL) return res;
	res.x = atoi(tok);
	
	tok = strtok(NULL, " ");
	if (tok == NULL) return res;
	res.y = atoi(tok);

	res.err = COORDS_PARSE_SUCCESS;
	return res;
}

int check_seat(table_t table, int x, int y)
{
	if (y <= 0 || y > table.dim.rows)
		return SEAT_DOES_NOT_EXIST;
	
	if (x <= 0 || x > table.dim.cols)
		return SEAT_DOES_NOT_EXIST;

	return table.data[x-1][y-1];
}

void reserve_seat(table_t table, int x, int y)
{
	if (y <= 0 || y > table.dim.rows)
		return;
	
	if (x <= 0 || x > table.dim.cols)
		return;

	table.data[x-1][y-1] = SEAT_RESERVED;
}

int isFull(table_t table)
{
	int p = 1;

	for(int i = 0; i < table.dim.rows; ++i)
	{
		for (int j = 0; j < table.dim.rows; ++j)
		{
			p &= table.data[i][j];
		}
	}

	return p;
}