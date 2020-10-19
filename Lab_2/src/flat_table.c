#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flat_table.h"
#include <time.h>

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

    for (unsigned int i = 0; i < table->size; i++)
    {
        if (imp__fget_line(line, FLAT_STRING_SIZE, file))
        {
            status = -3; // bad file
            printf("bad file!\n");
            break;
        }
        else if ((status = sread_flat(line, table->flats_array + i)) != 0)
        {
            printf("status was = %d. ", status);

            status = -4; // bad string
            printf("bad string! '%s'\n", line);
            break;
        }
    }

    return status;
}

flat_table_t ft_create(void)
{
    flat_table_t table = {
        .flats_array = NULL,
        .size = 0u
    };

    return table;
}

flat_table_t ft_clone(flat_table_t *original)
{
    flat_table_t copy = *original;

    copy.size = original->size;
    copy.flats_array = malloc(copy.size * sizeof(flat_t));

    for (uint32_t i = 0; i < copy.size; i++)
        copy.flats_array[i] = clone_flat(original->flats_array + i);

    return copy;
}

int ft_read(FILE *file, flat_table_t *table)
{
    // count flats amount
    unsigned int flats_amount = imp__count_flats_amount(file);
    if (flats_amount == 0)
        return -1; // no data was readed

    // allocate memory
    table->flats_array = (flat_t *)calloc(flats_amount, sizeof(flat_t));
    if (table->flats_array == NULL)
        return -2; // bad allocation

    table->size = flats_amount;

    // read file line by line
    if (imp__read_flat_table_from_file(file, table))
    {
        // could not read
        ft_free(table);
        return -3;
    }

    return 0;
}

int ft_append_flat(flat_table_t *table, flat_t *flat)
{
    // realloc array for new element
    table->size += 1;
    table->flats_array = (flat_t *)realloc(table->flats_array, table->size * sizeof(flat_t));
    table->flats_array[table->size - 1] = *flat;

    return 0;
}

void ft_gen_keys(flat_table_t *table, keys_table_t *keys)
{
    for (uint32_t i = 0; i < table->size; i++)
    {
        keys->keys[i].id = i;
        keys->keys[i].rooms_amount = table->flats_array[i].rooms_amount;
    }
}

bool ft_flat_satisfies(flat_t *flat, float price_1, float price_2)
{
    float price = flat->price_per_m2 * flat->area;
    return flat->type == SECONDARY && flat->rooms_amount == 2 && !flat->type_data.was_pets && price_1 <= price && price < price_2;
}

int ft_delete_flat(flat_table_t *table, unsigned int id, flat_t *deleted_flat)
{
    if (id >= table->size)
        return -1;
    
    *deleted_flat = table->flats_array[id];

    while (id + 1 < table->size)
    {
        table->flats_array[id] = table->flats_array[id + 1];
        id++;
    }

    table->size--; // realloc ?
    return 0;
}

void ft_free(flat_table_t *table)
{
    if (table == NULL)
        return;

    if (table->flats_array != NULL)
    {
        free(table->flats_array);
        table->flats_array = NULL;
        table->size = 0;
    }
}
