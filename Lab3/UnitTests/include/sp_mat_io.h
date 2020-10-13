/*

    Модуль для ввода-вывода разреженных матриц.

    Формат ввода матрицы из файла:
        
        #rows #cols
        0 0 0 0 1 0 0 0 2 -3
        3 0 0 0 0 4 0 0 0 0
        ...
        1 9 0 4 0 0 0 7 6 0
*/

#ifndef __SP_MAT_IO_H_
#define __SP_MAT_IO_H_

#include <stdio.h>
#include "sparse_matrix.h"

sparse_matrix_t smio_fread(FILE *file);
sparse_matrix_t smio_fread_fname(const char* fname);

void smio_fwrite(FILE *file, const sparse_matrix_t *matrix);

#endif
