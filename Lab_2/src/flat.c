#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include "flat.h"

// parser functions. Stops at ";" char or at the end of string

static int imp__parse_string(const char **str, char *out)
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

static int imp__parse_float(const char **str, float *number)
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

static int imp__parse_uchar(const char **str, unsigned char *number)
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

static int imp__parse_flat_type(const char **str, flat_t *flat)
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

static int imp__parse_uint8_t(const char **str, uint8_t *bit)
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

static int imp__parse_number(const char **str, int *num)
{
    char *end_ptr;

    // printf("before: str -> '%s'\n", *str);
    // printf("before: end -> '%s'\n", end_ptr);

    *num = strtol(*str, &end_ptr, 10);

    // printf("\nnum = %d\n", *num);
    // printf("then: str -> '%s'\n", *str);
    // printf("then: end -> '%s'\n", end_ptr);

    if (*num == 0 && (errno == EINVAL || *str == end_ptr))
        return -1;
    
    *str += (end_ptr - *str);
    // printf("after: '%s'\n", *str);
    return 0;
}

// expects time in format:
// dd.mm.yyyy
static int imp__parse_time_t(const char **str, time_t *time)
{
    assert(str != NULL);
    assert(*str != NULL);
    assert(time != NULL);

    int time_len = 0;
    for (const char *iter = *str; *iter != '\0' && *iter != FIELDS_DELIMETER; iter++)
        time_len++;

    int days = 0;
    int months = 0;
    int years = 0;

    const char *tmp_str = *str;
    if (imp__parse_number(&tmp_str, &days))
    {
        printf("bad number! day\n");
        return -1;
    }

    tmp_str++;
    if (imp__parse_number(&tmp_str, &months))
    {
        printf("bad number! mon\n");
        return -1;
    }

    tmp_str++;
    if (imp__parse_number(&tmp_str, &years))
    {
        printf("bad number! year\n");
        return -1;
    }

    if (days < 1 || 31 < days || months < 1 || 12 < months || years < 1880 || 2020 < years)
    {
        return -2;
    }

    struct tm tmstruct;
    memset((void*)&tmstruct, 0, sizeof(struct tm));

    tmstruct.tm_year = years;
    tmstruct.tm_mon = months - 1;
    tmstruct.tm_mday = days;

    *time = mktime(&tmstruct);

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
        status = imp__parse_uint8_t(str, &flat->type_data.has_trim);
    }
    else // secondary
    {
        status = imp__parse_uint8_t(str, &flat->type_data.was_pets) || imp__parse_time_t(str, &flat->build_time) || imp__parse_uchar(str, &flat->prev_owners_amount) || imp__parse_uchar(str, &flat->prev_lodgers_amount);
    }

    return status;
}

flat_t create_flat()
{
    flat_t flat = {
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

    int status = 0;

    status = imp__parse_string(&str, flat->address);
    if (status != 0)
    {
        printf("error in address '%s'\n", str);
        return status;
    }

    status = imp__parse_float(&str, &flat->area);
    if (status != 0)
    {
        printf("error in area: '%s'\n", str);
        return status;
    }
    
    status = imp__parse_uchar(&str, &flat->rooms_amount);
    if (status != 0)
    {
        printf("error in rooms amount: '%s'\n", str);
        return status;
    }
    
    status = imp__parse_float(&str, &flat->price_per_m2);
    if (status != 0)
    {
        printf("error in price: '%s'\n", str);
        return status;
    }
    
    status = imp__parse_flat_type(&str, flat);
    if (status != 0)
    {
        printf("error in type: '%s'\n", str);
        return status;
    }
    
    status = imp__parse_flat_type_data(&str, flat);
    // check for end of input
    if (status == 0 && strlen(str) != 0)
    {
        printf("end of string not reached!\n");
        status = -10; // end of string not reached
    }

    if (status != 0)
    {
        printf("bad enum data\n");
    }
    return status;
}

void printf_flat(uint32_t id, flat_t *flat)
{
    assert_flat(flat);

    printf("│ %04u │", id);
    printf(" %30s │", flat->address);
    printf(" %7.2f │", flat->area);
    printf(" %13d │", flat->rooms_amount);
    printf(" %9.2f │", flat->price_per_m2);
    printf(" %9s │", flat->type == PRIMARY ? PRIMARY_TYPE_STR : SECONDARY_TYPE_STR);

    if (flat->type == PRIMARY)
    {
        printf(" %7s │", flat->type_data.has_trim ? TRUE_STR : FALSE_STR);
        printf(" %8s │ %11s │ %15s │ %14s │", "", "", "", "");
    }
    else
    {
        printf(" %7s │", "");
        printf(" %8s │", flat->type_data.has_trim ? TRUE_STR : FALSE_STR);

        // print time
        char time_str[12];
        struct tm *_tm = localtime(&flat->build_time);

        // printf(" %11lu │", flat->build_time);
        sprintf(time_str, "%02d.%02d.%04d", _tm->tm_mday, _tm->tm_mon + 1, _tm->tm_year);
        printf(" %11s │", time_str);

        printf(" %15d │", flat->prev_owners_amount);
        printf(" %14d │", flat->prev_lodgers_amount);
    }

    printf("\n");
}

void assign_flat(void *flat_1, void *flat_2)
{
    *((flat_t *)flat_1) = *((flat_t *)flat_2);
}

void assign_flat_key(void *flat_1, void *flat_2)
{
    *((flat_key_t *)flat_1) = *((flat_key_t *)flat_2);
}
