#include <stdio.h>
#include <stdbool.h>
#include "sparse_matrix.h"
#include "nuklear_cross.h"

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 800

enum menu_opts {
    CHECK_MATRIX,
    MULTIPLY_MATRIX_VECTOR,
    MULTIPLY_MATRIX_MATRIX
};

struct my_nkc_app {
    struct nkc* nkcHandle;

    /* some user data */
    enum menu_opts opt;

    struct {
        struct {
            bool generated;
            mat_size_t input_rows;
            mat_size_t input_cols;
            sparse_matrix_t matrix;
        } check_matrix;
        struct {
            bool generated;
            mat_size_t input_rows;
            mat_size_t input_cols;
            sparse_matrix_t matrix;
            mat_elem_t* vector;
        } mult_mat_vec;
        struct {
            bool generated;
            mat_size_t input_rows;
            mat_size_t input_cols;
            mat_size_t input_cols_res;
            sparse_matrix_t matrix_1;
            sparse_matrix_t matrix_2;
            sparse_matrix_t result;
        } mult_mat_mat;
    } opt_data;
};

bool app_check_dims(int rows, int cols)
{
    bool rows_valid = 0 < rows && rows <= 200;
    bool cols_valid = 0 < cols && cols <= 200;

    return rows_valid && cols_valid;
}

void gui_check_matrix(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, u8"Введите значения матрицы здесь. Должно работать без проблем.", NK_TEXT_ALIGN_LEFT);

    nk_layout_row_dynamic(ctx, 0, 4);
    myapp->opt_data.check_matrix.input_rows = nk_propertyi(ctx, u8"строк", 1, myapp->opt_data.check_matrix.input_rows, 200, 1, 1);
    myapp->opt_data.check_matrix.input_cols = nk_propertyi(ctx, u8"столбцов", 1, myapp->opt_data.check_matrix.input_cols, 200, 1, 1);

    int rows = myapp->opt_data.check_matrix.input_rows;
    int cols = myapp->opt_data.check_matrix.input_cols;

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

    if (error_popup_active)
    {
        if (nk_popup_begin(ctx, NK_POPUP_STATIC, u8"Ошибка", 0, nk_rect(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, u8"Некорректные размеры матрицы", NK_TEXT_LEFT);
            if (nk_button_label(ctx, "OK")) {
                error_popup_active = false;
                nk_popup_close(ctx);
            }
            nk_popup_end(ctx);
        }
        else
            error_popup_active = false;
    }

    // output matrix
    if (myapp->opt_data.check_matrix.generated)
    {
        int mat_rows = myapp->opt_data.check_matrix.matrix.rows_size;
        int mat_cols = myapp->opt_data.check_matrix.matrix.cols_size;

        struct nk_rect rect = nk_window_get_content_region(ctx);

        nk_layout_row_dynamic(ctx, rect.h - 200, 1);
        // nk_layout_row_static(ctx, 0, 56, mat_cols + 1);
        if (nk_group_begin(ctx, u8"Матрица", NK_WINDOW_TITLE | NK_WINDOW_BORDER))
        {
            nk_layout_row_static(ctx, 0, 56, mat_cols + 1);
            nk_label(ctx, u8"[\\]", NK_TEXT_CENTERED);

            for (int col = 0; col < mat_cols; col++)
            {
                char col_str[10];
                sprintf(col_str, "%d", col);
                nk_label(ctx, col_str, NK_TEXT_CENTERED);
            }

            for (int row = 0; row < mat_rows; row++)
            {
                char row_str[10];
                sprintf(row_str, "%d", row);
                nk_label(ctx, row_str, NK_TEXT_CENTERED);

                for (int col = 0; col < mat_cols; col++)
                {
                    sparse_matrix_t* mat = &myapp->opt_data.check_matrix.matrix;

                    int value = sp_get(mat, row, col);
                    char text[20];
                    sprintf(text, "%d", value);
                    int text_len = strlen(text);

                    // int new_value = nk_propertyi(ctx, "", INT_MIN, value, INT_MAX, 1, 1);
                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text, &text_len, 6, nk_filter_decimal);
                    text[text_len] = '\0';

                    int new_value;
                    if ((sscanf(text, "%d", &new_value) == 1) && new_value != value)
                    {
                        sp_set(mat, row, col, new_value);
                        sp_print_info(mat);
                    }
                }
            }

            nk_group_end(ctx);
        }
        
    }
}

void gui_multiply_mat_vec(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
}

void gui_multiply_mat_mat(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
}

void mainLoop(void* loopArg) {
    struct my_nkc_app* myapp = (struct my_nkc_app*)loopArg;
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);

    union nkc_event e = nkc_poll_events(myapp->nkcHandle);
    if ((e.type == NKC_EWINDOW) && (e.window.param == NKC_EQUIT)) {
        nkc_stop_main_loop(myapp->nkcHandle);
    }

    /* Nuklear GUI code */
    if (nk_begin(ctx, "Lab3Window", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0)) {
        
        nk_layout_row_begin(ctx, NK_STATIC, 0, 1);
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, u8"МЕНЮ", NK_TEXT_LEFT, nk_vec2(220, 200)))
        {
            nk_layout_row_dynamic(ctx, 0, 1);
            if (nk_menu_item_label(ctx, u8"Проверка матрицы", NK_TEXT_LEFT))
            {
                if (myapp->opt == MULTIPLY_MATRIX_VECTOR)
                {
                    sp_free(&myapp->opt_data.mult_mat_vec.matrix);
                    if (myapp->opt_data.mult_mat_vec.vector != NULL)
                    {
                        free(myapp->opt_data.mult_mat_vec.vector);
                        myapp->opt_data.mult_mat_vec.vector = NULL;
                    }
                }
                else if (myapp->opt == MULTIPLY_MATRIX_MATRIX)
                {
                    sp_free(&myapp->opt_data.mult_mat_mat.matrix_1);
                    sp_free(&myapp->opt_data.mult_mat_mat.matrix_2);
                    sp_free(&myapp->opt_data.mult_mat_mat.result);
                }

                if (myapp->opt != CHECK_MATRIX)
                {
                    myapp->opt = CHECK_MATRIX;
                    myapp->opt_data.check_matrix.generated = false;
                    myapp->opt_data.check_matrix.input_rows = 1;
                    myapp->opt_data.check_matrix.input_cols = 1;
                    myapp->opt_data.check_matrix.matrix = sp_null_matrix();
                }
            }
            if (nk_menu_item_label(ctx, u8"Умножение на вектор", NK_TEXT_LEFT))
            {
                if (myapp->opt == CHECK_MATRIX)
                {
                    myapp->opt_data.check_matrix.generated = false;
                    myapp->opt_data.check_matrix.input_rows = 1;
                    myapp->opt_data.check_matrix.input_cols = 1;
                    myapp->opt_data.check_matrix.matrix = sp_null_matrix();
                }

                myapp->opt = MULTIPLY_MATRIX_VECTOR;

            }
            if (nk_menu_item_label(ctx, u8"Умножение на матрицу", NK_TEXT_LEFT))
                myapp->opt = MULTIPLY_MATRIX_MATRIX;

            nk_menu_end(ctx);
        }

        switch (myapp->opt)
        {
        default:
        case CHECK_MATRIX:
            gui_check_matrix(myapp);
            break;
        case MULTIPLY_MATRIX_VECTOR:
            gui_multiply_mat_vec(myapp);
            break;
        case MULTIPLY_MATRIX_MATRIX:
            gui_multiply_mat_mat(myapp);
            break;
        }
    }
    nk_end(ctx);
    /* End Nuklear GUI */

    nkc_render(myapp->nkcHandle, nk_rgb(40, 40, 40));
}

void init_myapp(struct my_nkc_app *myapp)
{
    myapp->opt = CHECK_MATRIX;
    myapp->opt_data.check_matrix.generated = false;
    myapp->opt_data.check_matrix.input_rows = 1;
    myapp->opt_data.check_matrix.input_cols = 1;
    myapp->opt_data.check_matrix.matrix = sp_null_matrix();

    myapp->opt_data.mult_mat_vec.generated = false;
    myapp->opt_data.mult_mat_vec.input_rows = 1;
    myapp->opt_data.mult_mat_vec.input_cols = 1;
    myapp->opt_data.mult_mat_vec.matrix = sp_null_matrix();
    myapp->opt_data.mult_mat_vec.vector = NULL;

    myapp->opt_data.mult_mat_mat.generated = false;
    myapp->opt_data.mult_mat_mat.input_rows = 1;
    myapp->opt_data.mult_mat_mat.input_cols = 1;
    myapp->opt_data.mult_mat_mat.input_cols_res = 1;
    myapp->opt_data.mult_mat_mat.matrix_1 = sp_null_matrix();
    myapp->opt_data.mult_mat_mat.matrix_2 = sp_null_matrix();
    myapp->opt_data.mult_mat_mat.result = sp_null_matrix();
}

int main() {
    struct my_nkc_app myapp;
    struct nkc nkcx; /* Allocate memory for Nuklear+ handle */
    myapp.nkcHandle = &nkcx;

    /* init some user data */
    init_myapp(&myapp);

    if (nkc_init(myapp.nkcHandle, "Nuklear+ Example", WINDOW_WIDTH, WINDOW_HEIGHT, NKC_WIN_NORMAL))
    {
        printf("Successfull init. Starting 'infinite' main loop...\n");
        {
            // setup custom font
            struct nk_user_font *font = nkc_load_font_file(myapp.nkcHandle, "Roboto-Regular.ttf", 18, nk_font_cyrillic_glyph_ranges());
            nkc_style_set_font(myapp.nkcHandle, font);
        }

        // set style
        nkc_set_style(myapp.nkcHandle, THEME_WHITE);
        nkc_set_main_loop(myapp.nkcHandle, mainLoop, (void*)&myapp);
    }
    else
    {
        printf("Can't init NKC\n");
    }

    nkc_shutdown(myapp.nkcHandle);
    return 0;
}
