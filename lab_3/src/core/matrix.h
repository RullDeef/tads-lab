#ifndef __MATRIX_H_
#define __MATRIX_H_

#include "sparse_matrix.h"
#include "dense_matrix.h"
#include "converter.h"

size_t mat_calc_sp_memsize(sparse_matrix_t *mat);
size_t mat_calc_dn_memsize(dense_matrix_t *mat);

float mat_calc_mem_eff(sparse_matrix_t *sp, dense_matrix_t *dn);
float mat_mem_eff(uint32_t rows, uint32_t cols, float percent);

#endif
