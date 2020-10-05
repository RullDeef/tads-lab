#ifndef __CONIO_H_
#define __CONIO_H_

#include <stdbool.h>
#include "sparse_matrix.h"

void con_clear(void);

void con_print_menu_header(void);
void con_print_opt(int opt, const char *text);
void con_print_menu_footer(void);

void con_wait(void);
bool con_confirm(bool default_yes);
bool con_ask(const char *msg, bool default_yes);
int con_get_numeric_opt(int min, int max);

// сюда же входит разделение на рандом (или нет?)
int con_input_matrix(sparse_matrix_t *matrix);
int con_input_vector(mat_elem_t *vector, mat_size_t size);

// better printer though...
void con_print_matrix(const sparse_matrix_t *matrix);
void con_print_vector(const mat_elem_t *vector, mat_size_t size);

#endif // __CONIO_H_
