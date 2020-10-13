#define _GNU_SOURCE
#include <stdio.h>
#include "sp_mat_io.h"

#define TEMP_BUFFER_LENGTH 512

sparse_matrix_t smio_fread(FILE* file)
{
    char line[TEMP_BUFFER_LENGTH];
    if (fgets(line, TEMP_BUFFER_LENGTH, file) == NULL)
        return sp_null_matrix();

    char* buf = line;
    size_t rows = strtoul(buf, &buf, 10);
    size_t cols = strtoul(buf, &buf, 10);

    if (rows == 0 || cols == 0)
        return sp_null_matrix();

    sparse_matrix_t matrix = sp_create(rows, cols);
    bool bad_mat = false;

    if (!sp_mat_is_null(&matrix))
    {
        for (size_t row = 0; !bad_mat && row < rows; row++)
        {
            if (fgets(line, TEMP_BUFFER_LENGTH, file) == NULL)
                bad_mat = true;
            else
            {
                buf = line;
                for (size_t col = 0; col < cols; col++)
                {
                    long value = strtol(buf, &buf, 10);
                    if (value != 0)
                        sp_set(&matrix, row, col, (mat_elem_t)value);
                }
            }
        }
    }

    if (bad_mat)
        sp_free(&matrix);
    
    return matrix;
}

sparse_matrix_t smio_fread_fname(const char* fname)
{
    FILE* file = fopen(fname, "rt");
    sparse_matrix_t matrix = smio_fread(file);
    fclose(file);
    return matrix;
}

void smio_fwrite(FILE* file, const sparse_matrix_t* matrix)
{
    if (matrix == NULL || sp_mat_is_null(matrix))
        return;
}
