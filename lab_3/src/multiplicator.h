#ifndef __MULTIPLICATOR_H_
#define __MULTIPLICATOR_H_

#include "core/matrix.h"

/**
 * @param time_1 - время плотного умножения     (тики)
 * @param time_2 - время разреженного умножения (тики)
 * @param ns_1 - время плотного умножения       (наносекунды)
 * @param ns_2 - время разреженного умножения   (наносекунды)
 */
int mult_mats(sparse_matrix_t *mat_1, sparse_matrix_t *mat_2, sparse_matrix_t *out, unsigned long long *ticks_1, unsigned long long *ticks_2, unsigned long long *ns_1, unsigned long long *ns_2);

#endif
