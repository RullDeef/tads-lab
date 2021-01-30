#include <stdio.h>
#include <stdlib.h>
#include "utils/str_parser.h"

int main(void)
{
    int32_t value = 0;

    if (parse_int32(" 20 10 \t\n", &value) == EXIT_SUCCESS)
        printf("success! %d\n", value);
    else
        printf("failure! %d\n", value);
    
    return 0;
}
