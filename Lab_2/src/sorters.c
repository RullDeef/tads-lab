#include "sorters.h"
#include <stdlib.h>

void insertion_sort(void *a, size_t count, size_t size, __comp_fn_t comp, __swap_fn_t swap)
{
#define A(i) (void *)((char *)a + size * (i))

    for (size_t i = count; i > 0; i--)
    {
        for (size_t j = 0; j + 1 < i; j++)
        {
            bool res = comp(A(j), A(j + 1)) < 0;
            if ((res && ascending) || (!res && !ascending))
                swap(A(j), A(j + 1));
        }
    }

#undef A
}

void merge_sort(void *a, size_t count, size_t size, __comp_fn_t comp, __assign_fn_t assign)
{
#define A(ar, id) (void *)((char *)ar + id * size)

    size_t rght, rend;
    size_t i, j, m;

    void *b = malloc(count * size);

    for (size_t k = 1; k < count; k *= 2)
    {
        for (size_t left = 0; left + k < count; left += k * 2)
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
                bool cmp = comp(A(a, i), A(a, j)) < 0;
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
