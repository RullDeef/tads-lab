#ifndef __OPERATIONS_H_
#define __OPERATIONS_H_

#include "flat.h"
#include "flat_table.h"

void pause();

int request_input_filename(char *fname);
int read_table_from_file(char *fname, flat_table_t *table);

int output_flat_table(flat_table_t *table, keys_table_t *keys);

int show_table(void *data);
int append_flat(void *data);
int delete_flat(void *data);
int search_flat(void *data);
int sort_table(void *data);

#endif // __OPERATIONS_H_
