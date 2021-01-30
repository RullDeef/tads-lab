#ifndef __CONVERTER_H_
#define __CONVERTER_H_

#include "sparse_matrix.h"
#include "dense_matrix.h"

dense_matrix_t cv_dense_from_sparse(sparse_matrix_t *sp_mat);
sparse_matrix_t cv_sparse_from_dense(dense_matrix_t *dn_mat);

#endif
