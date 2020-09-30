#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flat_table_common.h"

#define FLAT_STRING_SIZE 256

int imp__fget_line(char *str, int size, FILE *file)
{
    assert(str != NULL);
    assert(size > 0);
    assert(file != NULL);

    if (fgets(str, size, file) != str)
        return -1; // bad file
    
    // remove trailing \n & \r
    while (str[strlen(str) - 1] == '\n' || str[strlen(str) - 1] == '\r')
        str[strlen(str) - 1] = '\0';

    return 0;
}

unsigned int imp__count_flats_amount(FILE *file)
{
    assert(file != NULL);

    unsigned int amount = 0;

    rewind(file);
    for (char chr; (chr = fgetc(file)) != '\0' && chr != EOF;)
        if (chr == '\n')
            amount++;
    
    rewind(file);
    return amount;
}

int imp__read_flat_table_from_file(FILE *file, flat_table_t *table)
{
    char line[FLAT_STRING_SIZE];
    int status = 0;

    for (unsigned int i = 0; status == 0 && i < table->size; i++)
    {
        if (imp__fget_line(line, FLAT_STRING_SIZE, file))
            status = -3; // bad file
        
        else if (sread_flat(line, table->flats_array + i))
            status = -4; // bad string
    }

    return status;
}

flat_table_t create_flat_table()
{
    flat_table_t table = {
        .flats_array = NULL,
        .size = 0u
    };

    return table;
}

flat_table_t clone_flat_table(flat_table_t *original)
{
    assert_flat_table(original);

    flat_table_t copy = create_flat_table();

    copy.size = original->size;
    copy.flats_array = malloc(copy.size * sizeof(flat_t));

    for (int i = 0; i < copy.size; i++)
        copy.flats_array[i] = clone_flat(original->flats_array + i);

    return copy;
}

int fread_flat_table(FILE *file, flat_table_t *table)
{
    assert(file != NULL);
    assert(table != NULL);

    // count flats amount
    unsigned int flats_amount = imp__count_flats_amount(file);
    if (flats_amount == 0)
        return -1; // no data was readed

    // allocate memory
    table->flats_array = (flat_t*)malloc(flats_amount * sizeof(flat_t));
    if (table->flats_array == NULL)
        return -2; // bad allocation
    table->size = flats_amount;
    
    // read file line by line
    if (imp__read_flat_table_from_file(file, table))
    {
        // could not read
        free_flat_table(table);
        return -3;
    }

    return 0;
}

int fwrite_flat_table(FILE *file, flat_table_t *table)
{
    assert(file != NULL);
    assert_flat_table(table);

    for (unsigned int i = 0; i < table->size; i++)
        printf_flat(table->flats_array + i);

    return 0;
}

int append_flat_table(flat_table_t *table, flat_t *flat)
{
    assert_flat_table(table);
    assert_flat(flat);

    // realloc array for new element
    table->size += 1;
    table->flats_array = (flat_t *)realloc(table->flats_array, table->size);
    table->flats_array[table->size - 1] = *flat;

    return 0;
}

void imp__swap_flats(flat_t *flat_1, flat_t *flat_2)
{
    flat_t temp = *flat_1;
    *flat_1 = *flat_2;
    *flat_2 = temp;
}

int imp__flat_comp(flat_t *flat_1, flat_t *flat_2, sort_key_t key)
{
    switch (key)
    {
        case ADDRESS:
            return -strcmp(flat_1->address, flat_2->address);
        
        case AREA:
            if (flat_1->area < flat_2->area)
                return 1;
            else if (flat_1->area > flat_2->area)
                return -1;
            else
                return 0;
        
        case ROOMS_AMOUNT:
            return flat_2->rooms_amount - flat_1->rooms_amount;

        default:
            assert(0); // invalid key
            return 0;
    }
}

void sort_flat_table(flat_table_t *flat_table, sort_key_t key, bool ascending)
{
    assert_flat_table(flat_table);

    for (unsigned int i = flat_table->size; i > 0; i--)
    {
        for (unsigned int j = 0; j + 1 < i; j++)
        {
            flat_t *flat_1 = flat_table->flats_array + j;
            flat_t *flat_2 = flat_table->flats_array + j + 1;

            int res = imp__flat_comp(flat_1, flat_2, key);
            if ((res < 0 && ascending) || (res > 0 && !ascending))
                imp__swap_flats(flat_1, flat_2);
        }
    }
}

static bool imp__flat_satisfies(flat_t *flat, bit_t field_type, bit_t field_value)
{
    assert_flat(flat);
    return flat->type == field_type && flat->type_data.has_trim == field_value;
}

void search_flat_table(flat_table_t *table, bit_t field_type, bit_t field_value, flat_t **founded_flat)
{
    assert_flat_table(table);
    assert(field_type == PRIMARY || field_type == SECONDARY);
    assert(field_value == true || field_value == false);
    assert(founded_flat != NULL);

    *founded_flat = NULL;
    for (int i = 0; i < table->size; i++)
    {
        if (imp__flat_satisfies(table->flats_array + i, field_type, field_value))
        {
            *founded_flat = table->flats_array + i;
            break;
        }
    }
}

void free_flat_table(flat_table_t *table)
{
    if (table == NULL)
        return;

    if (table->flats_array != NULL)
    {
        for (unsigned int i = 0; i < table->size; i++)
            free_flat(table->flats_array + i);
        
        free(table->flats_array);
        table->flats_array = NULL;
        table->size = 0;
    }
}
