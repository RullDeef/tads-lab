/* nuklear - 1.32.0 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <errno.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <Nuklear/nuklear.h>
#include <Nuklear/nuklear_glfw_gl3.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
 /* This are some code examples to provide a small overview of what can be
  * done with this library. To try out an example uncomment the defines */
#define INCLUDE_ALL
  /*#define INCLUDE_STYLE */
  /*#define INCLUDE_CALCULATOR */
#define INCLUDE_OVERVIEW
/*#define INCLUDE_NODE_EDITOR */

#ifdef INCLUDE_ALL
#define INCLUDE_STYLE
#define INCLUDE_CALCULATOR
#define INCLUDE_OVERVIEW
#define INCLUDE_NODE_EDITOR
#endif

#ifdef INCLUDE_STYLE
#include "Nuklear/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
#include "Nuklear/calculator.c"
#endif
#ifdef INCLUDE_OVERVIEW
#include "Nuklear/overview.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
#include "Nuklear/node_editor.c"
#endif

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
static void error_callback(int e, const char* d)
{
    printf("Error %d: %s\n", e, d);
}

int main(void)
{
    /* Platform */
    struct nk_glfw glfw = { 0 };
    static GLFWwindow* win;
    int width = 0, height = 0;
    struct nk_context* ctx;
    struct nk_colorf bg;

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lab 3", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
        struct nk_font_atlas* atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);

        // add cyrillic range
        // struct nk_font_config config = nk_font_config(18);
        // config.range = nk_font_cyrillic_glyph_ranges();

        /*struct nk_font* roboto = nk_font_atlas_add_from_file(atlas, "Roboto-Regular.ttf", 18, &config);*/
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
        /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
        /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
        /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        nk_glfw3_font_stash_end(&glfw);
        // nk_style_load_all_cursors(ctx, atlas->cursors);
        //nk_style_set_font(ctx, &roboto->handle);
    }

#ifdef INCLUDE_STYLE
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    // set_style(ctx, THEME_DARK);
#endif

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (!glfwWindowShouldClose(win))
    {
        /* Input */
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        /* GUI */
        static int main_opt = 0;
        
        if (nk_begin(ctx, "Lab_3", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            // if (nk_option_label(ctx, "Проверка корректности заполнения матрицы", main_opt == 0)) main_opt = 0;
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

                static long rows = 0;
                static long cols = 0;
                
                static int invalid_dims = 0;

                if (nk_button_label(ctx, "generate"))
                {
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
                            printf("Generated matrix with dimensions: %ld x %ld\n", rows, cols);
                        }
                    }
                }

                if (invalid_dims)
                {
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
        */

        /* -------------- EXAMPLES ---------------- */
        overview(ctx);
        /*
#ifdef INCLUDE_CALCULATOR
        calculator(ctx);
#endif
#ifdef INCLUDE_OVERVIEW
#endif
#ifdef INCLUDE_NODE_EDITOR
        node_editor(ctx);
#endif
        */
        /* ----------------------------------------- */

        /* Draw */
        glfwGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);
    }
    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
    return 0;
}

