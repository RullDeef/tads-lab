#ifndef __MATRIX_INPUT_H_
#define __MATRIX_INPUT_H_

#include <stdbool.h>
#include "sparse_matrix.h"

bool input_auto_mat_dims(uint32_t *dim1, uint32_t *dim2, uint32_t *dim3);

sparse_matrix_t input_vector(uint32_t size);
sparse_matrix_t ultimate_input_matrix(void);

#endif
