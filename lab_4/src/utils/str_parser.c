#include <stdlib.h>
#include <string.h>
#include "status_codes.h"
#include "str_parser.h"

#include <ctype.h>

int parse_int32(const char *str, int32_t *value)
{
    while (*str != '\0' && isspace((int)*str))
        str++;

    if (*str == '\0')
        return EXIT_FAILURE;

    char *end = NULL;
    *value = strtol(str, &end, 10);

    if (end == NULL)
        return EXIT_FAILURE;

    while (*end != '\0' && isspace((int)*end))
        end++;
    
    if (*end == '\0')
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

bool parse_all_int32(char **str_arr, uint32_t len)
{
    int32_t value;

    for (uint32_t i = 0; i < len; i++)
        if (parse_int32(str_arr[i], &value) == EXIT_FAILURE)
            return false;

    return true;
}
