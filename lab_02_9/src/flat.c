#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "flat.h"
#include <stdbool.h>

// parser functions. Stops at ";" char or at the end of string

int imp__parse_string(const char **str, char *out)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(out != NULL);

    // skip first white spaces
    while (**str == ' ' || **str == '\t')
        (*str)++;

    int out_str_len = 0;
    for (const char *iter = *str; *iter != '\0' && *iter != FIELDS_DELIMETER; iter++)
        out_str_len++;

    if (out_str_len == 0 || out_str_len >= MAX_ADDRESS_SIZE)
        return -1; // bad str len

    memcpy(out, *str, out_str_len * sizeof(char));
    out[out_str_len] = '\0';

    // trim trailing white spaces
    while (out[strlen(out) - 1] == ' ' || out[strlen(out) - 1] == '\t')
        out[strlen(out) - 1] = '\0';

    // push forward str pointer
    *str += out_str_len + 1; // + 1 for delimiter
    return 0;
}

int imp__parse_float(const char **str, float *number)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(number != NULL);

    int num_len = 0;
    for (const char *iter = *str; *iter != '\0' && *iter != FIELDS_DELIMETER; iter++)
        num_len++;

    *number = strtof(*str, NULL);
    if (*number <= 0.0f)
        return -1; // bad number

    // push forward str pointer
    *str += num_len + 1; // + 1 for delimiter
    return 0;
}

int imp__parse_uchar(const char **str, unsigned char *number)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(number != NULL);

    int num_len = 0;
    for (const char *iter = *str; *iter != '\0' && *iter != FIELDS_DELIMETER; iter++)
        num_len++;

    unsigned long long_number = strtoul(*str, NULL, 10);
    if (long_number == 0ul)
        return -1; // bad number
    else if (long_number > 255ul)
        return -2; // too big number
    *number = (unsigned char)long_number;

    // push forward str pointer
    *str += num_len + 1; // + 1 for delimiter
    return 0;
}

int imp__parse_flat_type(const char **str, flat_t *flat)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(flat != NULL);

    char type_str[MAX_ADDRESS_SIZE];
    int status = imp__parse_string(str, type_str);
    if (status == 0)
    {
        if (strcmp(type_str, PRIMARY_TYPE_STR) == 0)
            flat->type = PRIMARY;
        else if (strcmp(type_str, SECONDARY_TYPE_STR) == 0)
            flat->type = SECONDARY;
        else
            status = -3; // invalid type string
    }

    return status;
}

int imp__parse_bit_t(const char **str, bit_t *bit)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(bit != NULL);

    char bit_str[MAX_ADDRESS_SIZE];
    int status = imp__parse_string(str, bit_str);
    if (status == 0)
    {
        if (strcmp(bit_str, TRUE_STR) == 0)
            *bit = 1u;
        else if (strcmp(bit_str, FALSE_STR) == 0)
            *bit = 0u;
        else
            status = -3; // bad has trim string
    }

    return status;
}

int imp__parse_time_t(const char **str, time_t *time)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(time != NULL);

    int time_len = 0;
    for (const char *iter = *str; *iter != '\0' && *iter != FIELDS_DELIMETER; iter++)
        time_len++;

    *time = strtoul(*str, NULL, 10);
    if (*time == 0ul)
        return -1; // bad number

    // push forward str pointer
    *str += time_len + 1; // + 1 for delimiter
    return 0;
}

int imp__parse_flat_type_data(const char **str, flat_t *flat)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(flat != NULL);

    int status = 0;

    if (flat->type == PRIMARY)
    {
        status = imp__parse_bit_t(str, &flat->type_data.has_trim);
    }
    else // secondary
    {
        status = imp__parse_bit_t(str, &flat->type_data.was_pets) || imp__parse_time_t(str, &flat->build_time) || imp__parse_uchar(str, &flat->prev_owners_amount) || imp__parse_uchar(str, &flat->prev_lodgers_amount);
    }

    return status;
}

flat_t create_flat()
{
    flat_t flat = {
        .id = 0,
        .address = "",
        .area = 0.0f,
        .rooms_amount = 0u,
        .price_per_m2 = 0.0f,
        .type = PRIMARY,
        .type_data = {
            .has_trim = false},
        .build_time = 0l,
        .prev_owners_amount = 0,
        .prev_lodgers_amount = 0};

    return flat;
}

flat_t clone_flat(flat_t *original)
{
    assert_flat(original);

    flat_t copy = *original;
    memcpy(copy.address, original->address, MAX_ADDRESS_SIZE);

    return copy;
}

int sread_flat(const char *str, flat_t *flat)
{
    assert(str != NULL);
    assert(flat != NULL);

    int status = imp__parse_string(&str, flat->address) || imp__parse_float(&str, &flat->area) || imp__parse_uchar(&str, &flat->rooms_amount) || imp__parse_float(&str, &flat->price_per_m2) || imp__parse_flat_type(&str, flat) || imp__parse_flat_type_data(&str, flat);

    // check for end of input
    if (status == 0 && strlen(str) != 0)
        status = -10; // end of string not reached

    return status;
}

void printf_flat(flat_t *flat)
{
    assert_flat(flat);

    printf("| %02d |", flat->id);
    printf(" %30s |", flat->address);
    printf(" %7.2f |", flat->area);
    printf(" %13d |", flat->rooms_amount);
    printf(" %9.2f |", flat->price_per_m2);
    printf(" %9s |", flat->type == PRIMARY ? PRIMARY_TYPE_STR : SECONDARY_TYPE_STR);

    if (flat->type == PRIMARY)
    {
        printf(" %7s |", flat->type_data.has_trim ? TRUE_STR : FALSE_STR);
        printf(" %8s | %11s | %15s | %14s |", "", "", "", "");
    }
    else
    {
        printf(" %7s |", "");
        printf(" %8s |", flat->type_data.has_trim ? TRUE_STR : FALSE_STR);
        printf(" %11lu |", flat->build_time);
        printf(" %15d |", flat->prev_owners_amount);
        printf(" %14d |", flat->prev_lodgers_amount);
    }

    printf("\n");
}

void swap_flat(void *flat_1, void *flat_2)
{
    flat_t temp = *(flat_t*)flat_1;
    *(flat_t *)flat_1 = *(flat_t *)flat_2;
    *(flat_t *)flat_2 = temp;
}

void swap_flat_ptr(void *flat_1, void *flat_2)
{
    flat_t *temp = *(flat_t **)flat_1;
    *(flat_t **)flat_1 = *(flat_t **)flat_2;
    *(flat_t **)flat_2 = temp;
}

void assign_flat(void *flat_1, void *flat_2)
{
    *((flat_t *)flat_1) = *((flat_t *)flat_2);
}

void assign_flat_ptr(void *flat_1, void *flat_2)
{
    *((flat_t **)flat_1) = *((flat_t **)flat_2);
}
