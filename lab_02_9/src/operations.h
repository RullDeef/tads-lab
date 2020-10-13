#ifndef __OPERATIONS_H_
#define __OPERATIONS_H_

#include "flat.h"
#include "flat_table.h"

#define MAX_FILE_NAME_LENGTH 128

void pause();

int request_input_filename(char *fname);
int read_table_from_file(char *fname, flat_table_t *table);

int output_flat_table(flat_table_t *table, flat_t **ptrs);
int append_flat_to_table(flat_table_t *table);

int sort_table(flat_table_t *table);
int search_flat(flat_table_t *table);

int delete_flat(flat_table_t *table);

#endif // __OPERATIONS_H_
