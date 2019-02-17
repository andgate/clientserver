#ifndef _TABLE_H_
#define _TABLE_H_


typedef struct table_dim {
	int rows;
	int cols;
} table_dim;

typedef struct table_t {
	table_dim dim;
	int** data;
} table_t;

table_t new_table(int rows, int cols);
void delete_table(table_t table);

void display_table(table_t table);
void dump_table(table_t table);

table_dim table_read_dim(char* str);

#endif