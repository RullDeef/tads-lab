#ifndef __MAIN_MENU_H_
#define __MAIN_MENU_H_

#include "flat.h"
#include "flat_table.h"

int request_input_filename(char *fname);
int read_table_from_file(char *fname, flat_table_t *table);

int show_table(void *data);
int append_flat(void *data);
int delete_flat(void *data);
int search_flat(void *data);

#endif // __MAIN_MENU_H_
