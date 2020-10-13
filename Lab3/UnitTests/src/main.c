#include <stdio.h>
#include "sparse_matrix.h"
#include "sp_mat_io.h"

int main(void)
{
    sparse_matrix_t mat = smio_fread_fname("res\\m10x10.txt");

    sp_print(&mat);
    sp_print_info(&mat);
    
    return 0;
};
