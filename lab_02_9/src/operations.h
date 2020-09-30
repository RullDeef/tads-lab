#ifndef __OPERATIONS_H_
#define __OPERATIONS_H_

#include "flat.h"
#include "flat_table_common.h"

#define MAX_FILE_NAME_LENGTH 128

typedef struct
{
    char input_filename[MAX_FILE_NAME_LENGTH];
    char output_filename[MAX_FILE_NAME_LENGTH];
    flat_table_t table;
} app_state_t;

app_state_t create_app_state();

int request_input_filename(app_state_t *state);
int request_output_filename(app_state_t *state);

int read_table_from_file(app_state_t *state);
int write_table_to_file(app_state_t *state);

int output_flat_table(app_state_t *state);
int append_flat_to_table(app_state_t *state);

int sort_table(app_state_t *state);
int search_flat(app_state_t *state);

void free_app_state(app_state_t *state);

#endif // __OPERATIONS_H_
