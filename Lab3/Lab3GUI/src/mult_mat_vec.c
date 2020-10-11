#include "myapp.h"
#include <math.h>

#define MAX_DIMS_STR_LEN 10

static void _imp__input_box(struct my_nkc_app *myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    if (nk_group_begin(ctx, u8"Ввод размеров матрицы", NK_WINDOW_BORDER))
    {
        int rows = myapp->opt_data.mult_mat_vec.input_rows;
        int cols = myapp->opt_data.mult_mat_vec.input_cols;

        // nk_layout_row_static(ctx, 0, 80, 6);
        float widths[] = { 120.0f, 80.0f, 120.0f, 80.0f, 80.0f, 120.0f, 120.0f, 80.0f, 120.0f };
        gui_make_row_layout(ctx, widths, 9);

        gui_input_dim_widget(ctx, u8"Строк", &rows);
        gui_input_dim_widget(ctx, u8"Столбцов", &cols);

        myapp->opt_data.mult_mat_vec.input_rows = rows;
        myapp->opt_data.mult_mat_vec.input_cols = cols;

        if (nk_button_label(ctx, u8"создать"))
        {
            if (app_check_dims(rows, cols))
            {
                sp_recreate(&myapp->opt_data.mult_mat_vec.matrix, rows, cols);
                if (myapp->opt_data.mult_mat_vec.vector != NULL)
                    free(myapp->opt_data.mult_mat_vec.vector);
                myapp->opt_data.mult_mat_vec.vector = calloc(cols, sizeof(mat_elem_t));

                // ERROR: при повторном создании матрицы и вектора возникает ошибка.
                if (myapp->opt_data.mult_mat_vec.result != NULL)
                    free(myapp->opt_data.mult_mat_vec.result);
                myapp->opt_data.mult_mat_vec.result = calloc(rows, sizeof(mat_elem_t));
                myapp->opt_data.mult_mat_vec.generated = true;
            }
            else
            {
                error_popup_active = true;
                myapp->opt_data.mult_mat_vec.generated = false;
            }
        }

        if (myapp->opt_data.mult_mat_vec.generated)
        {
            static int percent = 0;
            gui_input_dim_widget(ctx, u8"% заполнения", &percent);

            if (nk_button_label(ctx, u8"заполнить"))
            {
                sp_randomize(&myapp->opt_data.mult_mat_vec.matrix, (float)percent / 100.0f);
                // randomize vector too
                for (size_t i = 0; i < myapp->opt_data.mult_mat_vec.matrix.cols_size; i++)
                {
                    mat_elem_t value = rand() % 99 - 49;
                    if ((float)rand() / RAND_MAX < (float)percent / 100.0f)
                    {
                        if (value <= 0) value--;
                        myapp->opt_data.mult_mat_vec.vector[i] = value;
                    }
                    else
                        myapp->opt_data.mult_mat_vec.vector[i] = 0;
                }
            }

            if (nk_button_label(ctx, u8"Умножить"))
            {
                int status = sp_mult_by_vector(&myapp->opt_data.mult_mat_vec.matrix, myapp->opt_data.mult_mat_vec.vector, myapp->opt_data.mult_mat_vec.result);
                printf("mult mat vec status = %d\n", status);
            }
        }

        make_error_popup(ctx, &error_popup_active, u8"Некорректные размеры матрицы.");
        nk_group_end(ctx);
    }
}


void gui_multiply_mat_vec(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);

    nk_layout_row_dynamic(ctx, 0.0f, 1);
    nk_label(ctx, u8"Тестирование умножение матрицы на вектор.", NK_TEXT_ALIGN_LEFT);

    // read rows and cols
    nk_layout_row_dynamic(ctx, 52.0f, 1);
    _imp__input_box(myapp);

    // output matrix and vector
    if (myapp->opt_data.mult_mat_vec.generated)
    {
        int mat_rows = myapp->opt_data.mult_mat_vec.matrix.rows_size;
        int mat_cols = myapp->opt_data.mult_mat_vec.matrix.cols_size;

        struct nk_rect rect = nk_window_get_content_region(ctx);
        nk_layout_row_begin(ctx, NK_DYNAMIC, rect.h - 136.0f, 3);

        nk_layout_row_push(ctx, 0.6f);
        gui_matrix_out(ctx, u8"Матрица", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.mult_mat_vec.matrix, true);

        nk_layout_row_push(ctx, 0.2f);
        gui_vector_out(ctx, u8"Вектор", NK_WINDOW_TITLE | NK_WINDOW_BORDER, myapp->opt_data.mult_mat_vec.vector, mat_cols, true);

        nk_layout_row_push(ctx, 0.2f);
        gui_vector_out(ctx, u8"Произведение", NK_WINDOW_TITLE | NK_WINDOW_BORDER, myapp->opt_data.mult_mat_vec.result, mat_rows, false);

        nk_layout_row_end(ctx);
    }
}
