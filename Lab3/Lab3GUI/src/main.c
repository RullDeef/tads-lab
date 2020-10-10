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
            mat_elem_t* result;
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

void make_error_popup(struct nk_context *ctx, bool *active, const char *msg)
{
    if (*active)
    {
        if (nk_popup_begin(ctx, NK_POPUP_STATIC, u8"Ошибка", 0, nk_rect(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, msg, NK_TEXT_LEFT);
            if (nk_button_label(ctx, "OK"))
            {
                *active = false;
                nk_popup_close(ctx);
            }
            nk_popup_end(ctx);
        }
        else
            *active = false;
    }
}

bool gui_matrix_out(struct nk_context* ctx, const char* title, nk_flags flags, sparse_matrix_t *matrix, bool editable)
{
    bool changed = false;

    int mat_rows = matrix->rows_size;
    int mat_cols = matrix->cols_size;

    if (nk_group_begin(ctx, title, flags))
    {
        nk_layout_row_static(ctx, 0, 56, mat_cols + 1);
        nk_label(ctx, " ", NK_TEXT_CENTERED);

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
                int value = sp_get(matrix, row, col);
                char text[20];
                sprintf(text, "%d", value);
                int text_len = strlen(text);

                nk_edit_string(ctx, editable ? NK_EDIT_SIMPLE : NK_EDIT_READ_ONLY, text, &text_len, 6, nk_filter_decimal);
                text[text_len] = '\0';

                if (editable)
                {
                    int new_value;
                    if ((sscanf(text, "%d", &new_value) == 1) && new_value != value)
                    {
                        sp_set(matrix, row, col, new_value);
                        changed = true;
                    }
                }
            }
        }

        nk_group_end(ctx);
    }

    return changed;
}

bool gui_vector_out(struct nk_context* ctx, const char* title, nk_flags flags, mat_elem_t *vector, mat_size_t size, bool editable)
{
    bool changed = false;

    if (nk_group_begin(ctx, title, flags))
    {
        nk_layout_row_static(ctx, 0, 56, 2);
        nk_label(ctx, " ", NK_TEXT_CENTERED);
        nk_label(ctx, "0", NK_TEXT_CENTERED);

        for (int row = 0; row < size; row++)
        {
            char row_str[10];
            sprintf(row_str, "%d", row);
            nk_label(ctx, row_str, NK_TEXT_CENTERED);

            int value = vector[row];
            char text[20];
            sprintf(text, "%d", value);
            int text_len = strlen(text);
            
            nk_edit_string(ctx, editable ? NK_EDIT_SIMPLE : NK_EDIT_READ_ONLY, text, &text_len, 6, nk_filter_decimal);
            text[text_len] = '\0';

            if (editable)
            {
                int new_value;
                if ((sscanf(text, "%d", &new_value) == 1) && new_value != value)
                {
                    vector[row] = new_value;
                    changed = true;
                }
            }
        }

        nk_group_end(ctx);
    }

    return changed;
}

void gui_check_matrix(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, u8"Введите значения матрицы здесь. Должно работать без проблем.", NK_TEXT_ALIGN_LEFT);

    int rows = myapp->opt_data.check_matrix.input_rows;
    int cols = myapp->opt_data.check_matrix.input_cols;

    // read rows and cols
    {
        nk_layout_row_static(ctx, 0, 75, 2);
        nk_label(ctx, u8"Строк", NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED);

        char dims_str[10];
        int dims_str_len;

        sprintf(dims_str, "%d", rows);
        dims_str_len = strlen(dims_str);

        nk_edit_string(ctx, NK_EDIT_SIMPLE, dims_str, &dims_str_len, 10, nk_filter_decimal);
        dims_str[dims_str_len] = '\0';
        if (sscanf(dims_str, "%d", &rows) != 1)
            rows = 0;

        nk_layout_row_static(ctx, 0, 75, 3);
        sprintf(dims_str, "%d", cols);
        dims_str_len = strlen(dims_str);

        nk_label(ctx, u8"Столбцов", NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, dims_str, &dims_str_len, 10, nk_filter_decimal);
        dims_str[dims_str_len] = '\0';
        if (sscanf(dims_str, "%d", &cols) != 1)
            cols = 0;

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

        make_error_popup(ctx, &error_popup_active, u8"Некорректные размеры матрицы.");
    }

    // output matrix
    if (myapp->opt_data.check_matrix.generated)
    {
        int mat_rows = myapp->opt_data.check_matrix.matrix.rows_size;
        int mat_cols = myapp->opt_data.check_matrix.matrix.cols_size;

        struct nk_rect rect = nk_window_get_content_region(ctx);

        nk_layout_row_dynamic(ctx, rect.h - 200, 1);
        bool changed = gui_matrix_out(ctx, u8"Матрица", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.check_matrix.matrix, true);
        if (changed)
            sp_print_info(&myapp->opt_data.check_matrix.matrix);
    }
}

void gui_multiply_mat_vec(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
    static bool error_popup_active = false;

    nk_layout_row_dynamic(ctx, 0, 1);
    nk_label(ctx, u8"Тестирование умножение матрицы на вектор.", NK_TEXT_ALIGN_LEFT);

    int rows = myapp->opt_data.mult_mat_vec.input_rows;
    int cols = myapp->opt_data.mult_mat_vec.input_cols;

    // read rows and cols
    {
        nk_layout_row_static(ctx, 0, 75, 2);
        nk_label(ctx, u8"Строк", NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED);

        char dims_str[10];
        int dims_str_len;

        sprintf(dims_str, "%d", rows);
        dims_str_len = strlen(dims_str);

        nk_edit_string(ctx, NK_EDIT_SIMPLE, dims_str, &dims_str_len, 10, nk_filter_decimal);
        dims_str[dims_str_len] = '\0';
        if (sscanf(dims_str, "%d", &rows) != 1)
            rows = 0;

        nk_layout_row_static(ctx, 0, 75, 3);
        sprintf(dims_str, "%d", cols);
        dims_str_len = strlen(dims_str);

        nk_label(ctx, u8"Столбцов", NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, dims_str, &dims_str_len, 10, nk_filter_decimal);
        dims_str[dims_str_len] = '\0';
        if (sscanf(dims_str, "%d", &cols) != 1)
            cols = 0;

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
            nk_layout_row_static(ctx, 0, 120, 1);
            if (nk_button_label(ctx, u8"Перемножить"))
            {
                int status = sp_mult_by_vector(&myapp->opt_data.mult_mat_vec.matrix, myapp->opt_data.mult_mat_vec.vector, myapp->opt_data.mult_mat_vec.result);
                printf("mult mat vec status = %d\n", status);
            }
        }

        make_error_popup(ctx, &error_popup_active, u8"Некорректные размеры матрицы.");
    }

    // output matrix and vector
    if (myapp->opt_data.mult_mat_vec.generated)
    {
        int mat_rows = myapp->opt_data.mult_mat_vec.matrix.rows_size;
        int mat_cols = myapp->opt_data.mult_mat_vec.matrix.cols_size;

        struct nk_rect rect = nk_window_get_content_region(ctx);

        nk_layout_row_begin(ctx, NK_STATIC, rect.h - 200, 3);

        nk_layout_row_push(ctx, 0.485 * WINDOW_WIDTH);
        gui_matrix_out(ctx, u8"Матрица#2", NK_WINDOW_TITLE | NK_WINDOW_BORDER, &myapp->opt_data.mult_mat_vec.matrix, true);

        nk_layout_row_push(ctx, 0.225 * WINDOW_WIDTH);
        gui_vector_out(ctx, u8"Вектор", NK_WINDOW_TITLE | NK_WINDOW_BORDER, myapp->opt_data.mult_mat_vec.vector, mat_cols, true);

        nk_layout_row_push(ctx, 0.225 * WINDOW_WIDTH);
        gui_vector_out(ctx, u8"Произведение", NK_WINDOW_TITLE | NK_WINDOW_BORDER, myapp->opt_data.mult_mat_vec.result, mat_rows, false);

        nk_layout_row_end(ctx);
    }
}

void gui_multiply_mat_mat(struct my_nkc_app* myapp)
{
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);
}

void update_app_state(struct my_nkc_app* myapp, enum menu_opts opt)
{
    if (myapp->opt == opt)
        return;

    if (myapp->opt == MULTIPLY_MATRIX_VECTOR)
    {
        sp_free(&myapp->opt_data.mult_mat_vec.matrix);
        if (myapp->opt_data.mult_mat_vec.vector != NULL)
        {
            free(myapp->opt_data.mult_mat_vec.vector);
            myapp->opt_data.mult_mat_vec.vector = NULL;
        }
        if (myapp->opt_data.mult_mat_vec.result != NULL)
        {
            free(myapp->opt_data.mult_mat_vec.result);
            myapp->opt_data.mult_mat_vec.result = NULL;
        }
    }
    else if (myapp->opt == MULTIPLY_MATRIX_MATRIX)
    {
        sp_free(&myapp->opt_data.mult_mat_mat.matrix_1);
        sp_free(&myapp->opt_data.mult_mat_mat.matrix_2);
        sp_free(&myapp->opt_data.mult_mat_mat.result);
    }
    else if (myapp->opt == CHECK_MATRIX)
    {
        myapp->opt_data.check_matrix.generated = false;
        myapp->opt_data.check_matrix.input_rows = 1;
        myapp->opt_data.check_matrix.input_cols = 1;
        sp_free(&myapp->opt_data.check_matrix.matrix);
    }

    myapp->opt = opt;
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
                update_app_state(myapp, CHECK_MATRIX);
            if (nk_menu_item_label(ctx, u8"Умножение на вектор", NK_TEXT_LEFT))
                update_app_state(myapp, MULTIPLY_MATRIX_VECTOR);
            if (nk_menu_item_label(ctx, u8"Умножение на матрицу", NK_TEXT_LEFT))
                update_app_state(myapp, MULTIPLY_MATRIX_MATRIX);
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
    myapp->opt_data.mult_mat_vec.result = NULL;

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
            struct nk_user_font *font = nkc_load_font_file(myapp.nkcHandle, "SourceCodePro-Regular.ttf", 16, nk_font_cyrillic_glyph_ranges());
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
