#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flat_table.h"

#define FLAT_STRING_SIZE 256

#ifdef _PERF_TEST
    #include <time.h>
    #define BEGIN_TIMER clock_t __start_time = clock()
    #define END_TIMER(N) fprintf(stderr, "%d %d %lu\n", N, _PERF_TEST, clock() - __start_time)
#endif

typedef int (*__comp_fn_t)(void *, void *, sort_key_t);
typedef void (*__assign_fn_t)(void *, void *);
typedef void (*__swap_fn_t)(void *, void *);

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
            break;
        }
        else if (sread_flat(line, table->flats_array + i))
        {
            status = -4; // bad string
            break;
        }

        table->flats_array[i].id = i;
        table->flat_ptrs[i] = table->flats_array + i;
    }

    return status;
}

flat_table_t create_flat_table()
{
    flat_table_t table = {
        .flats_array = NULL,
        .flat_ptrs = NULL,
        .size = 0u
    };

    return table;
}

flat_table_t clone_flat_table(flat_table_t *original)
{
    assert_flat_table(original);

    flat_table_t copy = *original;

    copy.size = original->size;
    copy.flats_array = malloc(copy.size * sizeof(flat_t));
    copy.flat_ptrs = malloc(copy.size * sizeof(flat_t*));

    for (int i = 0; i < copy.size; i++)
    {
        copy.flats_array[i] = clone_flat(original->flats_array + i);
        copy.flat_ptrs[i] = copy.flats_array + (original->flat_ptrs[i] - original->flats_array);
    }

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
    table->flats_array = (flat_t *)calloc(flats_amount, sizeof(flat_t));
    if (table->flats_array == NULL)
        return -2; // bad allocation
    table->flat_ptrs = (flat_t **)calloc(flats_amount, sizeof(flat_t *));
    if (table->flat_ptrs == NULL) // bad allocation
    {
        free(table->flats_array);
        return -3;
    }
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
        printf_flat(table->flat_ptrs[i]);

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

static int imp__flat_comp(void *flat_1, void *flat_2, sort_key_t key)
{
    switch (key)
    {
        case ADDRESS:
            return -strcmp(((flat_t *)flat_1)->address, ((flat_t *)flat_2)->address);

        case AREA:
            if (((flat_t *)flat_1)->area < ((flat_t *)flat_2)->area)
                return 1;
            else if (((flat_t *)flat_1)->area > ((flat_t *)flat_2)->area)
                return -1;
            else
                return 0;
        
        case ROOMS_AMOUNT:
            return ((flat_t*)flat_2)->rooms_amount - ((flat_t*)flat_1)->rooms_amount;

        default:
            assert(0); // invalid key
            return 0;
    }
}

static int imp__flat_ptr_comp(void *flat_1p, void *flat_2p, sort_key_t key)
{
    return imp__flat_comp((void*)(*(flat_t**)flat_1p), (void*)(*(flat_t**)flat_2p), key);
}

static void imp__insertion(void *a, size_t count, size_t size, __comp_fn_t comp, __swap_fn_t swap, sort_key_t key, bool ascending)
{
    #define A(i) (void*)((char*)a + size * (i))

    for (size_t i = count; i > 0; i--)
    {
        for (size_t j = 0; j + 1 < i; j++)
        {
            bool res = comp(A(j), A(j + 1), key) < 0;
            if ((res && ascending) || (!res && !ascending))
                swap(A(j), A(j + 1));
        }
    }

    #undef A
}

static void imp__merge(void *a, size_t count, size_t size, __comp_fn_t comp, __assign_fn_t assign, sort_key_t key, bool ascending)
{
    #define A(ar,id) (void *)((char *)ar + id * size)

    int rght, rend;
    int i, j, m;

    void *b = malloc(count * size);

    for (int k = 1; k < count; k *= 2)
    {
        for (int left = 0; left + k < count; left += k * 2)
        {
            rght = left + k;
            rend = rght + k;
            if (rend > count)
                rend = count;
            m = left;
            i = left;
            j = rght;
            while (i < rght && j < rend)
            {
                bool cmp = comp(A(a, i), A(a, j), key) < 0;
                if ((cmp || ascending) && (!cmp || !ascending))
                {
                    assign(A(b, m), A(a, i)); // b[m] = a[i];
                    i++;
                }
                else
                {
                    assign(A(b, m), A(a, j)); // b[m] = a[j];
                    j++;
                }
                m++;
            }
            while (i < rght)
            {
                assign(A(b, m), A(a, i)); // b[m] = a[i];
                i++;
                m++;
            }
            while (j < rend)
            {
                assign(A(b, m), A(a, j)); // b[m] = a[j];
                j++;
                m++;
            }
            for (m = left; m < rend; m++)
                assign(A(a, m), A(b, m)); // a[m] = b[m];
        }
    }

    free(b);

    #undef A
}

void sort_flat_table_a_fast(flat_table_t *flat_table, sort_key_t key, bool ascending)
{
    assert_flat_table(flat_table);

#ifdef _PERF_TEST
    BEGIN_TIMER;
    for (size_t i = 0; i < _PERF_TEST; i++)
    {
        flat_table_t clone = clone_flat_table(flat_table);
        imp__merge(clone.flats_array, clone.size, sizeof(flat_t), imp__flat_comp, assign_flat, key, ascending);
        free_flat_table(&clone);
    }
    END_TIMER(flat_table->size);
#endif
    imp__merge(flat_table->flats_array, flat_table->size, sizeof(flat_t), imp__flat_comp, assign_flat, key, ascending);
}

void sort_flat_table_a_slow(flat_table_t *flat_table, sort_key_t key, bool ascending)
{
    assert_flat_table(flat_table);

#ifdef _PERF_TEST
    BEGIN_TIMER;
    for (size_t i = 0; i < _PERF_TEST; i++)
    {
        flat_table_t clone = clone_flat_table(flat_table);
        imp__insertion(clone.flats_array, clone.size, sizeof(flat_t), imp__flat_comp, swap_flat, key, ascending);
        free_flat_table(&clone);
    }
    END_TIMER(flat_table->size);
#endif
    imp__insertion(flat_table->flats_array, flat_table->size, sizeof(flat_t), imp__flat_comp, swap_flat, key, ascending);

    for (unsigned int i = 0; i < flat_table->size; i++)
        flat_table->flat_ptrs[i] = flat_table->flats_array + i;
}

void sort_flat_table_b_fast(flat_table_t *flat_table, sort_key_t key, bool ascending)
{
    assert_flat_table(flat_table);

#ifdef _PERF_TEST
    BEGIN_TIMER;
    for (size_t i = 0; i < _PERF_TEST; i++)
    {
        flat_table_t clone = clone_flat_table(flat_table);
        imp__merge(clone.flat_ptrs, clone.size, sizeof(flat_t*), imp__flat_ptr_comp, assign_flat_ptr, key, ascending);
        free_flat_table(&clone);
    }
    END_TIMER(flat_table->size);
#endif
    imp__merge(flat_table->flat_ptrs, flat_table->size, sizeof(flat_t*), imp__flat_ptr_comp, assign_flat_ptr, key, ascending);
}

void sort_flat_table_b_slow(flat_table_t *flat_table, sort_key_t key, bool ascending)
{
    assert_flat_table(flat_table);

#ifdef _PERF_TEST
    BEGIN_TIMER;
    for (size_t i = 0; i < _PERF_TEST; i++)
    {
        flat_table_t clone = clone_flat_table(flat_table);
        imp__insertion(clone.flat_ptrs, clone.size, sizeof(flat_t *), imp__flat_ptr_comp, swap_flat_ptr, key, ascending);
        free_flat_table(&clone);
    }
    END_TIMER(flat_table->size);
#endif
    imp__insertion(flat_table->flat_ptrs, flat_table->size, sizeof(flat_t*), imp__flat_ptr_comp, swap_flat_ptr, key, ascending);

    for (unsigned int i = 0; i < flat_table->size; i++)
        flat_table->flat_ptrs[i] = flat_table->flats_array + i;
}

static bool imp__flat_satisfies(flat_t *flat, float price_1, float price_2)
{
    assert_flat(flat);
    float price = flat->price_per_m2;
    return flat->type == SECONDARY && !flat->type_data.was_pets && price >= price_1 && price < price_2;
}

int search_flat_table(flat_table_t *table, float price_1, float price_2, search_callback_t callback)
{
    assert_flat_table(table);

    int founded = 0;

    for (int i = 0; i < table->size; i++)
    {
        if (imp__flat_satisfies(table->flat_ptrs[i], price_1, price_2))
        {
            callback(table->flat_ptrs[i]);
            founded++;
        }
    }

    return founded;
}

flat_t delete_flat_table(flat_table_t *table, unsigned int id)
{
    assert_flat_table(table);
    assert(0 <= id && id < table->size);

    flat_t removed_flat;
    flat_t *removed_flat_addr = NULL;

    size_t index = 0;
    while (index < table->size)
    {
        if (table->flats_array[index].id == id)
        {
            removed_flat = table->flats_array[index];
            removed_flat_addr = table->flats_array + index;
            break;
        }
        index++;
    }

    while (index + 1 < table->size)
    {
        table->flats_array[index] = table->flats_array[index + 1];
        index++;
    }

    // update pointers
    bool shift = false;
    for (size_t i = 0; i < table->size; i++)
    {
        if (table->flat_ptrs[i] == removed_flat_addr)
        {
            shift = true;
            continue;
        }
        
        if (table->flat_ptrs[i] > removed_flat_addr)
            table->flat_ptrs[i]--;
        
        if (shift)
        {
            table->flat_ptrs[i - 1] = table->flat_ptrs[i];
        }
    }

    // realloc
    table->size--;
    table->flats_array = realloc(table->flats_array, table->size * sizeof(flat_t));
    table->flat_ptrs = realloc(table->flat_ptrs, table->size * sizeof(flat_t*));

    return removed_flat;
}

void free_flat_table(flat_table_t *table)
{
    if (table == NULL)
        return;

    if (table->flat_ptrs != NULL)
    {
        free(table->flat_ptrs);
        table->flat_ptrs = NULL;
    }

    if (table->flats_array != NULL)
    {
        free(table->flats_array);
        table->flats_array = NULL;
        table->size = 0;
    }
}
