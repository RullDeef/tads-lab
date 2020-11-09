#include "multiplicator.h"
#include "utils/timing.h"

int mult_mats(sparse_matrix_t *mat_1, sparse_matrix_t *mat_2, sparse_matrix_t *out, unsigned long long *ticks_1, unsigned long long *ticks_2, unsigned long long *ns_1, unsigned long long *ns_2)
{
    if (ticks_1 != NULL)
    {
        dense_matrix_t dmat_1 = cv_dense_from_sparse(mat_1);
        dense_matrix_t dmat_2 = cv_dense_from_sparse(mat_2);
        dense_matrix_t dout = dn_null_matrix();

        BEGIN_TIMER;
        dn_mult_matrix(&dmat_1, &dmat_2, &dout);
        END_TIMER;
        *ticks_1 = TIMER_TICKS;
        if (ns_1 != NULL)
            *ns_1 = TIMER_NANOSECONDS;

        dn_free(&dmat_1);
        dn_free(&dmat_2);
        dn_free(&dout);
    }

    sp_transpose(mat_1);

    BEGIN_TIMER;
    int res = sp_mult_matrix(mat_1, mat_2, out);
    END_TIMER;
    if (ticks_2 != NULL)
        *ticks_2 = TIMER_TICKS;
    if (ns_2 != NULL)
        *ns_2 = TIMER_NANOSECONDS;

    return res;
}
