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


table_dim table_read_dim(char* str)
{
	table_dim dim;
	dim.rows = atoi(strtok(str, "x"));
	dim.cols = atoi(strtok(NULL, "x"));

	return dim;
}