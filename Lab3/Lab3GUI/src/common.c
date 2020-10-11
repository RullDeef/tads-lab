#include "myapp.h"

#define MAX_DIMS_STR_LEN 10
#define MAX_ELEM_STR_LEN 10

void init_myapp(struct my_nkc_app* myapp)
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

bool app_check_dims(int rows, int cols)
{
    bool rows_valid = 0 < rows && rows <= 200;
    bool cols_valid = 0 < cols && cols <= 200;

    return rows_valid && cols_valid;
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

void make_error_popup(struct nk_context* ctx, bool* active, const char* msg)
{
    if (*active)
    {
        if (nk_popup_begin(ctx, NK_POPUP_STATIC, u8"Îøèáêà", 0, nk_rect(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4)))
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

void gui_input_elem_widget(struct nk_context* ctx, int* elem)
{
    char elem_str[MAX_ELEM_STR_LEN];
    int elem_str_len;
    sprintf(elem_str, "%d", *elem);

    elem_str_len = strlen(elem_str);
    nk_edit_string(ctx, NK_EDIT_SIMPLE, elem_str, &elem_str_len, MAX_ELEM_STR_LEN, nk_filter_decimal);
    elem_str[elem_str_len] = '\0';

    int new_elem = 0;
    if (sscanf(elem_str, "%d", &new_elem) == 1)
    {
        if (*elem == 0)
            *elem = new_elem / 10;
        else
            *elem = new_elem;
    }
    else
        *elem = 0;
}

void gui_input_dim_widget(struct nk_context* ctx, const char* title, int* dim)
{
    nk_label(ctx, title, NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED);

    char dims_str[MAX_DIMS_STR_LEN];
    int dims_str_len;

    sprintf(dims_str, "%d", *dim);
    dims_str_len = strlen(dims_str);

    nk_edit_string(ctx, NK_EDIT_SIMPLE, dims_str, &dims_str_len, MAX_DIMS_STR_LEN, nk_filter_decimal);
    dims_str[dims_str_len] = '\0';

    int new_dim = 0;
    if (sscanf(dims_str, "%d", &new_dim) == 1)
    {
        if (*dim == 0)
            *dim = new_dim / 10;
        else
            *dim = new_dim;
    }
    else
        *dim = 0;
}

bool gui_matrix_out(struct nk_context* ctx, const char* title, nk_flags flags, sparse_matrix_t* matrix, bool editable)
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

bool gui_vector_out(struct nk_context* ctx, const char* title, nk_flags flags, mat_elem_t* vector, mat_size_t size, bool editable)
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
