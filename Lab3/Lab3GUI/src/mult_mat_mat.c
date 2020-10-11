#include "myapp.h"

void gui_multiply_mat_mat(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, u8"“естирование умножение матрицы на матрицу.", NK_TEXT_ALIGN_LEFT);


}
