#include "myapp.h"

static _imp__input_box(struct my_nkc_app *myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    if (nk_group_begin(ctx, u8"Ввод размеров матрицы", NK_WINDOW_BORDER))
    {
        int rows = myapp->opt_data.check_matrix.input_rows;
        int cols = myapp->opt_data.check_matrix.input_cols;

        // nk_layout_row_static(ctx, 0, 80, 5);
        float widths[] = { 120.0f, 80.0f, 120.0f, 80.0f, 80.0f, 120.0f, 80.0f, 120.0f };
        gui_make_row_layout(ctx, widths, 8);

        gui_input_dim_widget(ctx, u8"Строк", &rows);
        gui_input_dim_widget(ctx, u8"Столбцов", &cols);

        myapp->opt_data.check_matrix.input_rows = rows;
        myapp->opt_data.check_matrix.input_cols = cols;

        if (nk_button_label(ctx, u8"создать"))
        {
            if (app_check_dims(rows, cols))
            {
                sp_recreate(&myapp->opt_data.check_matrix.matrix, rows, cols);
                myapp->opt_data.check_matrix.generated = true;
            }
            else
            {
                error_popup_active = true;
                myapp->opt_data.check_matrix.generated = false;
            }
        }

        if (myapp->opt_data.check_matrix.generated)
        {
            static int percent = 0;
            gui_input_dim_widget(ctx, u8"% заполнения", &percent);

            if (nk_button_label(ctx, u8"заполнить"))
                sp_randomize(&myapp->opt_data.check_matrix.matrix, (float)percent / 100.0f);
        }

        make_error_popup(ctx, &error_popup_active, u8"Некорректные размеры матрицы.");
        nk_group_end(ctx);
    }
}

void gui_check_matrix(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, u8"Введите значения матрицы здесь. Должно работать без проблем.", NK_TEXT_ALIGN_LEFT);

    nk_layout_row_dynamic(ctx, 52.0f, 1);
    _imp__input_box(myapp);

    // output matrix
    if (myapp->opt_data.check_matrix.generated)
    {
        int mat_rows = myapp->opt_data.check_matrix.matrix.rows_size;
        int mat_cols = myapp->opt_data.check_matrix.matrix.cols_size;

        struct nk_rect rect = nk_window_get_content_region(ctx);

        nk_layout_row_dynamic(ctx, rect.h - 136.0f, 1);
        bool changed = gui_matrix_out(ctx, u8"Матрица", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.check_matrix.matrix, true);
        if (changed)
            sp_print_info(&myapp->opt_data.check_matrix.matrix);
    }
}

