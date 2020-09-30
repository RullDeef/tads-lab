#ifndef BIG_FLOAT_H_
#define GIB_FLOAT_H_

#include "status_codes.h"

#ifndef DIGIT_PACKS_AMOUNT
#define DIGIT_PACKS_AMOUNT 15
#endif

enum big_float_sign_enum
{
    POSITIVE = 1,
    NEGATIVE = -1
};

typedef struct big_float_t
{
    char mantissa_sign; // POSITIVE or NEGATIVE
    unsigned char mantissa[15]; // decimal digit pairs
    int power;
} big_float_t;

// == UTILS OPERATIONS == //

// void reset_big_float(big_float_t *big_float);

// MATH OPERATIONS == //

status_code_t divide_big_float(big_float_t *dividend, big_float_t *divisor, big_float_t *output);

// == INPUT / OUTPUT == //

// status_code_t parse_big_float(const char *float_str, big_float_t *big_float, short int *pos);
status_code_t input_big_float(big_float_t *big_float, short int *pos);
status_code_t float_to_big_float(float float_num, big_float_t *big_float, short int* pos);
float big_float_to_float(big_float_t* big_float);

// void big_float_to_str(big_float_t *big_float, char *str);
void output_big_float(big_float_t *big_float);

#endif
