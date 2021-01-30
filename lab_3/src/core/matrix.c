#include "matrix.h"

size_t mat_calc_sp_memsize(sparse_matrix_t *mat)
{
    size_t size = 0U;

    size += sizeof(mat->rows_size);
    size += sizeof(mat->cols_size);
    size += sizeof(mat->nonzero_size);
    size += sizeof(mat->__alloc_nz_sz);
    size += sizeof(mat->__alloc_cl_sz);
    size += sizeof(mat->cols);
    size += sizeof(mat->rows);
    size += sizeof(mat->nonzero_array);
    size += sizeof(*(mat->cols)) * mat->__alloc_cl_sz;
    if (mat->__alloc_nz_sz > 0)
        size += sizeof(*(mat->rows)) * mat->__alloc_nz_sz;
    if (mat->__alloc_nz_sz > 0)
        size += sizeof(*(mat->nonzero_array)) * mat->__alloc_nz_sz;

    return size;
}

static size_t __mat_calc_sp_mem(uint32_t rows, uint32_t cols, float percent)
{
    size_t size = 0U;
    sparse_matrix_t mat;

    percent = percent > 1.0f ? 1.0 : percent < 0.0f ? 0.0f : percent;

    size += sizeof(mat.rows_size);
    size += sizeof(mat.cols_size);
    size += sizeof(mat.nonzero_size);
    // size += sizeof(mat.__alloc_nz_sz);
    // size += sizeof(mat.__alloc_cl_sz);
    size += sizeof(mat.cols);
    size += sizeof(mat.rows);
    size += sizeof(mat.nonzero_array);
    size += sizeof(*(mat.cols)) * (cols + 1);

    size_t non_zero = rows * cols * percent;
    size += sizeof(mat_elem_t *) * non_zero;
    size += sizeof(mat_elem_t) * non_zero;

    return size;
}

size_t mat_calc_dn_memsize(dense_matrix_t *mat)
{
    size_t size = 0U;

    size += sizeof(mat->rows_size);
    size += sizeof(mat->cols_size);
    size += sizeof(mat->data);

    if (mat->rows_size * mat->cols_size > 0)
    {
        size += sizeof(*(mat->data)) * mat->rows_size;
        size += sizeof(**(mat->data)) * mat->rows_size * mat->cols_size;
    }

    return size;
}

static size_t __mat_calc_dn_mem(uint32_t rows, uint32_t cols)
{
    size_t size = 0U;
    dense_matrix_t mat;

    size += sizeof(mat.rows_size);
    size += sizeof(mat.cols_size);
    size += sizeof(mat.data);

    size += sizeof(mat_elem_t *) * rows;
    size += sizeof(mat_elem_t) * rows * cols;

    return size;
}

float mat_calc_mem_eff(sparse_matrix_t *sp, dense_matrix_t *dn)
{
    size_t size_sp = mat_calc_sp_memsize(sp);
    size_t size_dn = mat_calc_dn_memsize(dn);

    return ((float)size_dn - size_sp) / size_dn;
}

float mat_mem_eff(uint32_t rows, uint32_t cols, float percent)
{
    size_t sp = __mat_calc_sp_mem(rows, cols, percent);
    size_t dn = __mat_calc_dn_mem(rows, cols);

    return ((float)dn - sp) / dn;
}
