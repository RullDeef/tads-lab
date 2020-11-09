#include <assert.h>
#include "sparse_matrix.h"
#include "dense_matrix.h"
#include "converter.h"

dense_matrix_t cv_dense_from_sparse(sparse_matrix_t *sp_mat)
{
    dense_matrix_t dn_mat = dn_null_matrix();

    if (!sp_mat_is_null(sp_mat))
    {
        dn_mat = dn_create(sp_mat->rows_size, sp_mat->cols_size);
        assert(!dn_mat_is_null(&dn_mat));

        uint32_t col = 0;
        for (uint32_t index = 0; index < sp_mat->nonzero_size; index++)
        {
            while (sp_mat->cols[col + 1] <= index)
                col++;
            
            uint32_t row = sp_mat->rows[index];
            dn_mat.data[row][col] = sp_mat->nonzero_array[index];
        }
    }

    return dn_mat;
}

sparse_matrix_t cv_sparse_from_dense(dense_matrix_t *dn_mat)
{
    sparse_matrix_t sp_mat = sp_null_matrix();

    if (!dn_mat_is_null(dn_mat))
    {
        sp_mat = sp_create(dn_mat->rows_size, dn_mat->cols_size);
        assert(!sp_mat_is_null(&sp_mat));

        uint32_t index = 0;
        for (uint32_t col = 0; col < dn_mat->cols_size; col++)
        {
            for (uint32_t row = 0; row < dn_mat->rows_size; row++)
            {
                if (dn_mat->data[row][col] != 0)
                {
                    // realloc if needed
                    if (sp_mat.__alloc_nz_sz <= index)
                    {
                        sp_mat.__alloc_nz_sz = (index + 1) * SP_ALLOC_MULTIPILER;
                        sp_mat.nonzero_array = realloc(sp_mat.nonzero_array, sp_mat.__alloc_nz_sz * sizeof(int));
                        sp_mat.rows = realloc(sp_mat.rows, sp_mat.__alloc_nz_sz * sizeof(int));
                        assert(sp_mat.nonzero_array != NULL && sp_mat.rows != NULL);
                    }

                    sp_mat.nonzero_array[index] = dn_mat->data[row][col];
                    sp_mat.rows[index] = row;
                    sp_mat.cols[col + 1]++;

                    index++;
                }
            }
        }

        // adjust cols
        for (uint32_t col = 0; col < sp_mat.cols_size; col++)
            sp_mat.cols[col + 1] += sp_mat.cols[col];
        sp_mat.nonzero_size = index;
    }

    return sp_mat;
}
