#ifndef _TABLE_H_
#define _TABLE_H_

#define COORDS_PARSE

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


// Parse coordinates
#define COORDS_PARSE_SUCCESS 0
#define COORDS_PARSE_FAILURE 1

typedef struct coords_parse_result {
	int x;
	int y;
	int err;
} coords_parse_result;

coords_parse_result table_read_coords(char* str);


// Check seat in the table
#define SEAT_AVAILABLE       0
#define SEAT_RESERVED        1
#define SEAT_DOES_NOT_EXIST  2

int check_seat(table_t table, int x, int y);
void reserve_seat(table_t table, int x, int y);

#endif