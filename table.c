#include "table.h"

#include <stdlib.h>
#include <stdio.h>

Table newTable(int rows, int cols)
{
	if (rows <= 0 || cols <= 0)
	{
		fprintf(stderr, "Table rows and/or columns must be larger than 0.\n");
		exit(EXIT_FAILURE);
	}

	Table table;
	table.rows = rows;
	table.cols = cols;
	
	// Allocate rows
	table.data = (int**) malloc((rows-1)*sizeof(int*));

	// Allocate columns
	for(int i = 0; i < rows; ++i)
		table.data[i] = (int*) calloc(cols, sizeof(int));

	return table;
}

void deleteTable(Table table)
{
	// Delete columns
	for(int i = 0; i < table.rows; ++i)
		free(table.data[i]);

	// Delete rows
	free(table.data);
}

void displayTable(Table table)
{
	printf("  ");
	for(int i = 0; i < table.cols; ++i)
		printf("%d ", i+1);
	printf("\n");

	for(int i = 0; i < table.rows; ++i)
	{
		printf("%d ", i+1);
		for(int j = 0; j < table.cols; ++j)
		{
			if(!table.data[i][j])
				printf("o ");
			else
				printf("x ");
		}
		printf("\n");
	}
}

void dumpTable(Table table)
{
	printf("%dx%d-Table\n", table.rows, table.cols);

	for(int i = 0; i < table.rows; ++i)
	{
		for(int j = 0; j < table.cols; ++j)
		{
			printf("table[%d][%d] = %d\n", i, j, table.data[i][j]);
		}
	}
}