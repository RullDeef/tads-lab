#include "myapp.h"

static _imp__input_box(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    if (nk_group_begin(ctx, u8"Ввод размеров матриц", NK_WINDOW_BORDER))
    {
        int rows = myapp->opt_data.mult_mat_mat.input_rows;
        int cols = myapp->opt_data.mult_mat_mat.input_cols;
        int cols_res = myapp->opt_data.mult_mat_mat.input_cols_res;

        float widths[] = { 120.0f, 60.0f, 120.0f, 60.0f, 120.0f, 60.0f, 80.0f, 120.0f, 60.0f, 90.0f, 90.0f };
        gui_make_row_layout(ctx, widths, 11);
        
        gui_input_dim_widget(ctx, u8"Строк 1-й", &rows);
        gui_input_dim_widget(ctx, u8"Столбцов 1-й", &cols);
        gui_input_dim_widget(ctx, u8"Столбцов 2-й", &cols_res);

        myapp->opt_data.mult_mat_mat.input_rows = rows;
        myapp->opt_data.mult_mat_mat.input_cols = cols;
        myapp->opt_data.mult_mat_mat.input_cols_res = cols_res;

        if (nk_button_label(ctx, u8"создать"))
        {
            if (app_check_dims(rows, cols) && app_check_dims(cols, cols_res))
            {
                sp_recreate(&myapp->opt_data.mult_mat_mat.matrix_1, rows, cols);
                sp_recreate(&myapp->opt_data.mult_mat_mat.matrix_2, cols, cols_res);
                sp_recreate(&myapp->opt_data.mult_mat_mat.result, rows, cols_res);
                myapp->opt_data.mult_mat_mat.generated = true;
            }
            else
            {
                error_popup_active = true;
                myapp->opt_data.mult_mat_mat.generated = false;
            }
        }

        if (myapp->opt_data.mult_mat_mat.generated)
        {
            static int percent = 0;
            gui_input_dim_widget(ctx, u8"% заполнения", &percent);

            if (nk_button_label(ctx, u8"заполнить"))
            {
                sp_randomize(&myapp->opt_data.mult_mat_mat.matrix_1, (float)percent / 100.0f);
                sp_randomize(&myapp->opt_data.mult_mat_mat.matrix_2, (float)percent / 100.0f);
            }

            if (nk_button_label(ctx, u8"Умножить"))
            {
                int status = sp_mult_matrix(&myapp->opt_data.mult_mat_mat.matrix_1, &myapp->opt_data.mult_mat_mat.matrix_2, &myapp->opt_data.mult_mat_mat.result);
                printf("mult mat vec status = %d\n", status);
            }
        }

        make_error_popup(ctx, &error_popup_active, u8"Некорректные размеры матриц(ы).");
        nk_group_end(ctx);
    }
}

void gui_multiply_mat_mat(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, u8"Тестирование умножение матрицы на матрицу.", NK_TEXT_ALIGN_LEFT);

    nk_layout_row_dynamic(ctx, 52.0f, 1);
    _imp__input_box(myapp);

    // output matrices
    if (myapp->opt_data.mult_mat_mat.generated)
    {
        struct nk_rect rect = nk_window_get_content_region(ctx);
        nk_layout_row_dynamic(ctx, rect.h - 136.0f, 1);

        gui_matrix_out(ctx, u8"Матрица 1", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.mult_mat_mat.matrix_1, true);
        gui_matrix_out(ctx, u8"Матрица 2", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.mult_mat_mat.matrix_2, true);
        gui_matrix_out(ctx, u8"Произведение", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.mult_mat_mat.result, false);
    }
}
