#ifndef _TABLE_H_
#define _TABLE_H_

typedef struct Table {
	int rows;
	int cols;
	int** data;
} Table;

Table newTable(int rows, int cols);
void deleteTable(Table table);

void displayTable(Table table);
void dumpTable(Table table);

#endif