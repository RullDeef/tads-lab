#include <stdlib.h>
#include <stdio.h>
#include "../include/conio.h"

inline void con_clear()
{
    system("clear");
}

inline void con_print_menu_header()
{
    printf("-- [меню] --\n\n");
}

inline void con_print_opt(int opt, const char *text)
{
    printf("  %2d. %s\n", opt, text);
}

inline void con_print_menu_footer()
{
    printf("\n");
}

void con_wait(void)
{
    char temp[2];
    fgets(temp, 2, stdin);
}

bool con_confirm(bool default_yes)
{
    printf(default_yes ? "(Y/n): " : "(y/N): ");

    char line[64];
    if (fgets(line, 64, stdin) == line)
    {
        if (line[0] == 'y' || line[0] == 'Y')
            return true;
        else if (line[0] == 'n' || line[0] == 'N')
            return false;
    }
    
    return default_yes;
}

bool con_ask(const char *msg, bool default_yes)
{
    printf("%s ", msg);
    return con_confirm(default_yes);
}

int con_get_numeric_opt(int min, int max)
{
    printf("[%d-%d]: ", min, max);

    int opt;
    if (scanf("%d", &opt) != 1 || opt < min || opt > max)
        opt = -1;

    return opt;
}

// сюда же входит разделение на рандом (или нет?)
int con_input_matrix(sparse_matrix_t *matrix);
int con_input_vector(mat_elem_t **vector, mat_size_t *size);

// better printer though...
void con_print_matrix(sparse_matrix_t *matrix);
