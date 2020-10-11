#ifndef __MYAPP_H_
#define __MYAPP_H_

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
            size_t input_rows;
            size_t input_cols;
            sparse_matrix_t matrix;
        } check_matrix;
        struct {
            bool generated;
            size_t input_rows;
            size_t input_cols;
            sparse_matrix_t matrix;
            mat_elem_t* vector;
            mat_elem_t* result;
        } mult_mat_vec;
        struct {
            bool generated;
            size_t input_rows;
            size_t input_cols;
            size_t input_cols_res;
            sparse_matrix_t matrix_1;
            sparse_matrix_t matrix_2;
            sparse_matrix_t result;
        } mult_mat_mat;
    } opt_data;
};

void init_myapp(struct my_nkc_app* myapp);
bool app_check_dims(int rows, int cols);

void update_app_state(struct my_nkc_app* myapp, enum menu_opts opt);
void make_error_popup(struct nk_context* ctx, bool* active, const char* msg);

void gui_make_row_layout(struct nk_context* ctx, float widths[], int size);

void gui_input_elem_widget(struct nk_context* ctx, int* elem);
void gui_input_dim_widget(struct nk_context* ctx, const char* title, int* dim);

bool gui_matrix_out(struct nk_context* ctx, const char* title, nk_flags flags, sparse_matrix_t* matrix, bool editable);
bool gui_vector_out(struct nk_context* ctx, const char* title, nk_flags flags, mat_elem_t* vector, size_t size, bool editable);

void gui_check_matrix(struct my_nkc_app* myapp);
void gui_multiply_mat_vec(struct my_nkc_app* myapp);
void gui_multiply_mat_mat(struct my_nkc_app* myapp);

#endif
