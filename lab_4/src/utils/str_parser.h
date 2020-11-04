#ifndef __STR_PARSER_H_
#define __STR_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

int parse_int32(const char *str, int32_t *value);
bool parse_all_int32(char **str_arr, uint32_t len);

#endif
