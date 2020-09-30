#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "status_codes.h"

#include "big_float/big_float.h"

#define TESTS_AMOUNT 20

float rand_float(float min, float max)
{
    return min + ((float)rand()) / (float)RAND_MAX * max;
}

int main()
{
    printf("Testing with %i digits.\n", 2 * DIGIT_PACKS_AMOUNT);
    srand(time(NULL));

    float max_float = 1.0e+10;
    float min_float = 1.0;
    for (int i = 0; i < TESTS_AMOUNT; i++)
    {
        float a = rand_float(min_float, max_float);
        float b = rand_float(min_float, max_float);

        // printf("a = %.6g, b = %f\n", a, b);

        float c = a / b;

        big_float_t bf_a, bf_b, bf_c, bf_f;
        if (float_to_big_float(a, &bf_a, NULL) ||
            float_to_big_float(b, &bf_b, NULL) ||
            float_to_big_float(c, &bf_c, NULL))
        {
            printf("Error while parsing floats %f and %f!\n", a, b);
            return -1;
        }

        // output_big_float(&bf_a);
        // output_big_float(&bf_b);

        if (divide_big_float(&bf_a, &bf_b, &bf_f))
        {
            printf("cant divide floats: %f / %f\n", a, b);
            return -2;
        }

        printf("Test #%i:\n  ", i);
        output_big_float(&bf_c);
        printf("\n  ");
        output_big_float(&bf_f);
        printf("\n");
    }

    printf("All random tests passed!\n");

    return 0;
}