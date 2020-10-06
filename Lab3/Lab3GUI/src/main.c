#include <stdio.h>
#include <stdbool.h>
#include "sparse_matrix.h"
#include "nuklear_cross.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

enum menu_opts {
    CHECK_MATRIX,
    MULTIPLY_MATRIX_VECTOR,
    MULTIPLY_MATRIX_MATRIX
};

struct my_nkc_app {
    struct nkc* nkcHandle;

    /* some user data */
    enum menu_opts opt;

    union {
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
    nk_label(ctx, "Test matrix input here. It supposed to work without any problems.", NK_TEXT_ALIGN_LEFT);

    nk_layout_row_dynamic(ctx, 0, 4);
    myapp->opt_data.check_matrix.input_rows = nk_propertyi(ctx, "rows", 1, myapp->opt_data.check_matrix.input_rows, 200, 1, 1);
    myapp->opt_data.check_matrix.input_cols = nk_propertyi(ctx, "cols", 1, myapp->opt_data.check_matrix.input_cols, 200, 1, 1);

    int rows = myapp->opt_data.check_matrix.input_rows;
    int cols = myapp->opt_data.check_matrix.input_cols;

    if (nk_button_label(ctx, "generate"))
    {
        if (app_check_dims(rows, cols))
        {
            sp_free(&myapp->opt_data.check_matrix.matrix);
            myapp->opt_data.check_matrix.matrix = sp_create(rows, cols);
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
        if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Error", 0, nk_rect(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Invalid dimentions", NK_TEXT_LEFT);
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

        nk_layout_row_static(ctx, 0, 56, mat_cols + 1);
        nk_label(ctx, "row\\col", NK_TEXT_CENTERED);

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
                int new_value = nk_propertyi(ctx, "", INT_MIN, value, INT_MAX, 1, 1);

                if (new_value != value)
                {
                    sp_set(mat, row, col, new_value);
                    sp_print_info(mat);
                }
            }
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
        if (nk_menu_begin_label(ctx, "MENU", NK_TEXT_LEFT, nk_vec2(120, 200)))
        {
            nk_layout_row_dynamic(ctx, 0, 1);
            if (nk_menu_item_label(ctx, "Check matrix", NK_TEXT_LEFT))
            {
                if (myapp->opt == MULTIPLY_MATRIX_VECTOR)
                {
                    sp_free(&myapp->opt_data.mult_mat_vec.matrix);
                    free(myapp->opt_data.mult_mat_vec.vector);
                }
                else if (myapp->opt == MULTIPLY_MATRIX_MATRIX)
                {
                    sp_free(&myapp->opt_data.mult_mat_mat.matrix_1);
                    sp_free(&myapp->opt_data.mult_mat_mat.matrix_2);
                    sp_free(&myapp->opt_data.mult_mat_mat.result);
                }

                myapp->opt = CHECK_MATRIX;
                myapp->opt_data.check_matrix.generated = false;
                myapp->opt_data.check_matrix.input_rows = 1;
                myapp->opt_data.check_matrix.input_cols = 1;
                myapp->opt_data.check_matrix.matrix = sp_null_matrix();
            }
            if (nk_menu_item_label(ctx, "Mat X vect", NK_TEXT_LEFT))
                myapp->opt = MULTIPLY_MATRIX_VECTOR;
            if (nk_menu_item_label(ctx, "Mat X mat", NK_TEXT_LEFT))
                myapp->opt = MULTIPLY_MATRIX_MATRIX;

            // nk_progress(ctx, &prog, 100, NK_MODIFIABLE);
            // nk_slider_int(ctx, 0, &slider, 16, 1);
            // nk_checkbox_label(ctx, "check", &check);

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
        /*
        // fixed widget pixel width
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button")) {
            // event handling
            printf("Button pressed\n");
        }

        // fixed widget window ratio width
        nk_layout_row_dynamic(ctx, 0, 2);
        if (nk_option_label(ctx, "easy", myapp->opt == 0)) myapp->opt = 0;
        if (nk_option_label(ctx, "very hard", myapp->opt == 1)) myapp->opt = 1;

        // custom widget pixel width
        nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push(ctx, 50);
            nk_label(ctx, "Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 110);
            // nk_slider_float(ctx, 0, &(myapp->value), 1.0f, 0.1f);
        }
        nk_layout_row_end(ctx);
        */
    }
    nk_end(ctx);
    /* End Nuklear GUI */

    nkc_render(myapp->nkcHandle, nk_rgb(40, 40, 40));
}

int main() {
    struct my_nkc_app myapp;
    struct nkc nkcx; /* Allocate memory for Nuklear+ handle */
    myapp.nkcHandle = &nkcx;

    /* init some user data */
    myapp.opt = CHECK_MATRIX;
    myapp.opt_data.check_matrix.generated = false;
    myapp.opt_data.check_matrix.input_rows = 1;
    myapp.opt_data.check_matrix.input_cols = 1;
    myapp.opt_data.check_matrix.matrix = sp_null_matrix();

    if (nkc_init(myapp.nkcHandle, "Nuklear+ Example", WINDOW_WIDTH, WINDOW_HEIGHT, NKC_WIN_NORMAL))
    {
        printf("Successfull init. Starting 'infinite' main loop...\n");
        {
            // setup custom font
            //struct nk_user_font *font = nkc_load_font_file(myapp.nkcHandle, "Roboto-Regular.ttf", 14, nk_font_cyrillic_glyph_ranges());
            //nkc_style_set_font(myapp.nkcHandle, font);
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

/*
int main(void)
{
    struct nk_glfw glfw = { 0 };
    static GLFWwindow* win;
    int width = 0, height = 0;
    struct nk_context* ctx;
    struct nk_colorf bg;

    { // test sparse matrix realisation
        sparse_matrix_t mat = sp_create(40, 20);
        sp_free(&mat);
    }

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lab 3", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);
    {
        struct nk_font_atlas* atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);

        // add cyrillic range
        // struct nk_font_config config = nk_font_config(18);
        // config.range = nk_font_cyrillic_glyph_ranges();
        nk_glfw3_font_stash_end(&glfw);
        // nk_style_load_all_cursors(ctx, atlas->cursors);
        //nk_style_set_font(ctx, &roboto->handle);
    }


    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        static int main_opt = 0;
        
        if (nk_begin(ctx, "Lab_3", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            // if (nk_option_label(ctx, "ѕроверка корректности заполнени€ матрицы", main_opt == 0)) main_opt = 0;
            if (nk_option_label(ctx, "Examine matrix to be readed correctly", main_opt == 0)) main_opt = 0;
            if (nk_option_label(ctx, "Multiply matrix by vector", main_opt == 1)) main_opt = 1;
            if (nk_option_label(ctx, "Multiply matrix by matrix", main_opt == 2)) main_opt = 2;

            if (main_opt == 0)
            {
                static char row_str[11] = "0";
                static int row_str_len = 1;

                static char col_str[11] = "0";
                static int col_str_len = 1;

                const float sizes[] = { 60, 60, 60, 60, 120 };
                nk_layout_row(ctx, NK_STATIC, 25, 5, sizes);

                nk_label(ctx, "rows:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, row_str, &row_str_len, 10, NK_FILTER_INT);

                nk_label(ctx, "cols:", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, col_str, &col_str_len, 10, NK_FILTER_INT);

                static long rows = 0, cols = 0;
                static int invalid_dims = 0;

                static sparse_matrix_t sparse_matrix;
                static bool matrix_generated = false;

                // generate matrix wit hnew dimensions
                if (nk_button_label(ctx, "generate"))
                {
                    // обнулили предыдущий вариант матрицы
                    sp_free(&sparse_matrix);
                    sparse_matrix = sp_null_matrix();

                    char* end_str;

                    row_str[row_str_len] = '\0';
                    col_str[col_str_len] = '\0';

                    rows = strtol(row_str, &end_str, 10);

                    if (rows == 0 && (errno == EINVAL || end_str == row_str))
                        invalid_dims = 1;
                    
                    else
                    {
                        cols = strtol(col_str, &end_str, 10);

                        if (cols == 0 && (errno == EINVAL || end_str == col_str))
                            invalid_dims = 1;

                        else if (rows <= 0 || cols <= 0)
                            invalid_dims = 1;

                        else
                        {
                            // создали новую матрицу
                            sparse_matrix = sp_create(rows, cols);
                            matrix_generated = true;
                            printf("Generated matrix with dimensions: %ld x %ld\n", rows, cols);
                            sp_print_info(&sparse_matrix);
                        }
                    }
                }

                // show error msg popup
                if (invalid_dims)
                {
                    matrix_generated = false;
                    static struct nk_rect s = { 20, 100, 220, 90 };
                    if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Error", 0, s))
                    {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "Invalid dimensions", NK_TEXT_CENTERED);
                        if (nk_button_label(ctx, "OK")) {
                            invalid_dims = 0;
                            nk_popup_close(ctx);
                        }
                        nk_popup_end(ctx);
                    }
                    else invalid_dims = 0;
                }

                if (matrix_generated)
                {
                    // print input boxes for all dimmensions
                    nk_layout_row_static(ctx, 40, 400, 1);
                    nk_label(ctx, "Generated sparse matrix:", NK_TEXT_LEFT);
                    
                    nk_layout_row_static(ctx, 0, 56, cols + 1);
                    nk_label(ctx, "row\\col", NK_TEXT_CENTERED);
                    for (mat_index_t col = 0; col < cols; col++)
                    {
                        char col_num[10];
                        sprintf(col_num, "%ld", col);
                        nk_label(ctx, col_num, NK_TEXT_CENTERED);
                    }

                    for (mat_index_t row = 0; row < rows; row++)
                    {
                        char row_num[10];
                        sprintf(row_num, "%ld", row);
                        nk_label(ctx, row_num, NK_TEXT_CENTERED);

                        for (mat_index_t col = 0; col < cols; col++)
                        {
                            int value = (int)sp_get(&sparse_matrix, row, col);
                            int new_value = nk_propertyi(ctx, "", INT_MIN, value, INT_MAX, 1, 1);
                            if (new_value != value)
                            {
                                sp_set(&sparse_matrix, row, col, (mat_elem_t)new_value);
                                printf("Matrix value updated!\n");
                                sp_print_info(&sparse_matrix);
                            }
                        }
                    }

                    nk_layout_row_static(ctx, 40, 400, 1);
                    nk_label(ctx, "Try to edit some of the values. Nothing bad must happened.", NK_TEXT_LEFT);


                }
            }
        }
        nk_end(ctx);

        /*
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum { EASY, HARD };
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        overview(ctx);
        

        glfwGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);
    }
    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
    return 0;
}

*/