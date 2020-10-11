#include <stdio.h>
#include <stdbool.h>
#include "myapp.h"

void mainLoop(void* loopArg)
{
    struct my_nkc_app* myapp = (struct my_nkc_app*)loopArg;
    struct nk_context* ctx = nkc_get_ctx(myapp->nkcHandle);

    union nkc_event e = nkc_poll_events(myapp->nkcHandle);
    if ((e.type == NKC_EWINDOW) && (e.window.param == NKC_EQUIT))
        nkc_stop_main_loop(myapp->nkcHandle);

    /* Nuklear GUI code */
    if (nk_begin(ctx, "Lab3Window", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), 0))
    {
        nk_layout_row_begin(ctx, NK_STATIC, 0, 1);
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, u8"МЕНЮ", NK_TEXT_CENTERED, nk_vec2(220, 200)))
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


int main() {
    struct my_nkc_app myapp;
    struct nkc nkcx; /* Allocate memory for Nuklear+ handle */
    myapp.nkcHandle = &nkcx;

    /* init some user data */
    init_myapp(&myapp);

    if (nkc_init(myapp.nkcHandle, u8"Лабораторная работа №3: интерактивная программа", WINDOW_WIDTH, WINDOW_HEIGHT, NKC_WIN_FIXED))
    {
        printf("Successfull init. Starting 'infinite' main loop...\n");

        // setup custom font
        struct nk_user_font *font = nkc_load_font_file(myapp.nkcHandle, "SourceCodePro-Regular.ttf", 16, nk_font_cyrillic_glyph_ranges());
        nkc_style_set_font(myapp.nkcHandle, font);

        // set style
        nkc_set_style(myapp.nkcHandle, THEME_BLUE);
        nkc_set_main_loop(myapp.nkcHandle, mainLoop, (void*)&myapp);
    }
    else
        printf("Can't init NKC\n");

    nkc_shutdown(myapp.nkcHandle);
    return 0;
}
