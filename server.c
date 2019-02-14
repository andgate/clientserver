#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "util.h"


int main(int argc, char* argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Not enough arguments.\n");
		exit(EXIT_FAILURE);
	}

	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);
	Table table = newTable(rows, cols);

	dumpTable(table);
	displayTable(table);

	Config cfg = readConfig();
}