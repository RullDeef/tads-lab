#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "big_float/big_float.h"

#include <assert.h>

#define ENLARGE_PACKS_AMOUNT 5
#define MAX_INPUT_STRING_LENGTH 255

/**
 *  @brief Присваивает числовой структуре значение +0.0E0.
 * 
 *  @param big_float - числовая структура.
 */
void reset_big_float(big_float_t *big_float)
{

    assert(big_float != NULL); // big_float must not be NULL

    big_float->mantissa_sign = POSITIVE;
    memset(big_float->mantissa, 0, DIGIT_PACKS_AMOUNT);
    big_float->power = 0;
}

bool big_float_is_zero(big_float_t *big_float)
{

    assert(big_float != NULL); // big_float must not be NULL

    for (int i = 0; i < DIGIT_PACKS_AMOUNT; i++)
        if (big_float->mantissa[i] != 0)
            return false;
    return true;
}

bool big_float_is_one(big_float_t *big_float)
{

    assert(big_float != NULL); // big_float must not be NULL

    if (big_float->mantissa_sign == NEGATIVE)
        return false;
    if (big_float->mantissa[0] != 10)
        return false;
    for (int i = 1; i < DIGIT_PACKS_AMOUNT; i++)
        if (big_float->mantissa[i] != 0)
            return false;
    return big_float->power == 1;
}

int _compare_enlarged_mantisses(unsigned char *mantissa_1, unsigned char *mantissa_2)
{

    assert(mantissa_1 != NULL); // mantissa_1 must not be NULL
    assert(mantissa_2 != NULL); // mantissa_2 must not be NULL

    for (short i = 0; i < DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT; i++)
        if (mantissa_1[i] != mantissa_2[i])
            return mantissa_1[i] - mantissa_2[i];
    return 0;
}

/**
 *  @brief Умножает каждую компоненту мантиссы на заданную цифру.
 * 
 *  При переполнении оставляет излишек в нулевой компоненте.
 * 
 *  @param mantissa - исходная мантисса,
 *  @param scalar - множитель [0-9],
 *  @param output - результирующее произведение.
 * 
 *  @return true - there was overflow
 */
bool _multscalar_enlarged_mantissa(unsigned char *mantissa, short scalar, unsigned char *output)
{
    assert(mantissa != NULL);           // mantissa must not be NULL
    assert(output != NULL);             // output must not be NULL
    assert(0 <= scalar && scalar < 10); // scalar range [0-9]

    short overflow_bit = 0;
    for (short i = DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT - 1; i >= 0; i--)
    {
        short digit = mantissa[i] * scalar + overflow_bit;
        if (digit >= 100 && i > 0)
        {
            overflow_bit = digit / 100;
            digit %= 100;
        }
        else
            overflow_bit = 0;

        output[i] = digit;
        if (i == 0 && digit > 100)
            return true;
    }
    return false;
}

// возвращает число [0-9] - максимальный множитель k, такой, что
// dividend > k * divisor
unsigned char _get_multipiler(unsigned char *dividend, unsigned char *divisor)
{
    assert(dividend != NULL); // dividend must not be NULL
    assert(divisor != NULL);  // divisor must not be NULL

    unsigned char output[DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT];
    bool overflow = _multscalar_enlarged_mantissa(divisor, 4, output);

    // go bin tree here
    if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 4 is too big
    {
        overflow = _multscalar_enlarged_mantissa(divisor, 2, output);

        if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 2 is too big
            return 1;
        else // 2 fits
        {
            overflow = _multscalar_enlarged_mantissa(divisor, 3, output);

            if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 3 is too big
                return 2;
            else // 3 fits
                return 3;
        }
    }
    else // 4 fits
    {
        overflow = _multscalar_enlarged_mantissa(divisor, 8, output);

        if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 8 is too big
        {
            overflow = _multscalar_enlarged_mantissa(divisor, 6, output);

            if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 6 is too big
            {
                overflow = _multscalar_enlarged_mantissa(divisor, 5, output);

                if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 5 is too big
                    return 4;
                else // 5 fits
                    return 5;
            }
            else // 6 fits
            {
                overflow = _multscalar_enlarged_mantissa(divisor, 7, output);

                if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 7 is too big
                    return 6;
                else // 7 fits
                    return 7;
            }
        }
        else // 8 fits
        {
            overflow = _multscalar_enlarged_mantissa(divisor, 9, output);

            if (overflow || _compare_enlarged_mantisses(dividend, output) < 0) // 9 is too big
                return 8;
            else // 9 fits
                return 9;
        }
    }

    return 0;
}

// true если все цифры мантиссы - нули
bool _enlarged_mantissa_is_zero(unsigned char *mantissa)
{
    assert(mantissa != NULL); // mantissa must not be NULL

    for (short i = 0; i < DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT; i++)
        if (mantissa[i] != 0u)
            return false;
    return true;
}

// сдвигает мантиссу на 1 цифру вправо (на месте)
void _shift_enlarged_mantissa(unsigned char *mantissa)
{
    assert(mantissa != NULL); // mantissa must not be NULL

    for (short i = DIGIT_PACKS_AMOUNT; i >= 0; i--)
    {
        mantissa[i] /= 10;
        if (i > 0)
            mantissa[i] += 10 * (mantissa[i - 1] % 10);
    }
}

unsigned char _get_mantissa_digit(unsigned char *mantissa, char index)
{
    if (index % 2 == 0)
        return mantissa[index / 2] / 10;
    else
        return mantissa[index / 2] % 10;
}

// index - [0-(2*DIGIT_PACKS_AMOUNT-1)], digit - [0-9]
void _set_mantissa_digit(unsigned char *mantissa, char index, unsigned char digit)
{
    assert(mantissa != NULL);                             // mantissa must not be NULL
    assert(0 <= index && index < 2 * DIGIT_PACKS_AMOUNT); // index range [0-29]
    assert(0 <= digit && digit < 20);                     // digit range [0-9]

    if (digit >= 10)
    {
        _set_mantissa_digit(mantissa, index - 1, _get_mantissa_digit(mantissa, index - 1) + 1);
        digit -= 10;
    }

    if (index % 2 == 0)
        mantissa[index / 2] = 10 * digit + mantissa[index / 2] % 10;
    else
        mantissa[index / 2] += digit - mantissa[index / 2] % 10;
}

int _count_digits(big_float_t *big_float)
{
    assert(big_float != NULL);

    int digits = 2 * DIGIT_PACKS_AMOUNT;

    while (big_float->mantissa[digits / 2 - 1] == 0)
        digits -= 2;

    if (big_float->mantissa[digits / 2 - 1] % 10 == 0)
        digits--;

    return digits;
}

// mantissa_1 -= weight * mantissa_2
// гарантировано, что результат будет неотрицательным
void _subtract_weighted_enlargeed_mantisses(unsigned char *mantissa_1, unsigned char *mantissa_2, char weight)
{
    assert(mantissa_1 != NULL);         // mantissa_1 must not be NULL
    assert(mantissa_2 != NULL);         // mantissa_2 must not be NULL
    assert(0 <= weight && weight < 10); // weight range [0-9]

    if (weight == 0) // nothing to subtract
        return;

    short take = 0;
    for (short i = DIGIT_PACKS_AMOUNT; i >= 0; i--)
    {
        short digit = mantissa_1[i] - weight * mantissa_2[i] - take;
        if (digit < 0)
        {

            assert(i > 0); // bad assumption!

            take = (digit % 100 == 0 ? 0 : 1) - digit / 100;
            digit += 100 * take;
        }
        else
            take = 0;
        mantissa_1[i] = digit;
    }
}

void _divide_mantisses(unsigned char *dividend, unsigned char *divisor, unsigned char *output)
{
    assert(dividend != NULL); // dividend must not be NULL
    assert(divisor != NULL);  // divisor must not be NULL
    assert(output != NULL);   // output must not be NULL

    unsigned char dividend_copy[DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT];
    unsigned char divisor_copy[DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT];
    memset(dividend_copy, 0, DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT);
    memset(divisor_copy, 0, DIGIT_PACKS_AMOUNT + ENLARGE_PACKS_AMOUNT);
    memcpy(dividend_copy, dividend, DIGIT_PACKS_AMOUNT);
    memcpy(divisor_copy, divisor, DIGIT_PACKS_AMOUNT);

    if (_compare_enlarged_mantisses(dividend_copy, divisor_copy) < 0) // dividend < divisor
        _shift_enlarged_mantissa(divisor_copy);

    char digit_index = 0;
    while (!_enlarged_mantissa_is_zero(divisor_copy) && digit_index < 2 * DIGIT_PACKS_AMOUNT)
    {
        if (_compare_enlarged_mantisses(dividend_copy, divisor_copy) < 0) // dividend < divisor
        {
            _shift_enlarged_mantissa(divisor_copy);
            _set_mantissa_digit(output, digit_index, 0);
        }
        else // dividend >= divisor
        {
            char weight = _get_multipiler(dividend_copy, divisor_copy);
            _subtract_weighted_enlargeed_mantisses(dividend_copy, divisor_copy, weight);
            _set_mantissa_digit(output, digit_index, weight);
            _shift_enlarged_mantissa(divisor_copy);
        }

        digit_index++;
    }

    if (digit_index == 2 * DIGIT_PACKS_AMOUNT)
    {
        char weight = _get_multipiler(dividend_copy, divisor_copy);
        if (weight >= 5)
            _set_mantissa_digit(output, digit_index - 1, _get_mantissa_digit(output, digit_index - 1) + 1);
    }
    else // fill zeros
        while (digit_index < 2 * DIGIT_PACKS_AMOUNT)
            _set_mantissa_digit(output, digit_index++, 0);
}

status_code_t divide_big_float(big_float_t *dividend, big_float_t *divisor, big_float_t *output)
{
    assert(dividend != NULL);                                               // dividend must not be NULL
    assert(divisor != NULL);                                                // divisor must not be NULL
    assert(output != NULL);                                                 // output must not be NULL
    assert(dividend != divisor && divisor != output && dividend != output); // no same refs

    if (big_float_is_zero(divisor)) // must not divide by zero
        return DIVISION_BY_ZERO;

    if (big_float_is_zero(dividend))
    {
        reset_big_float(output);
        return SUCCESS;
    }

    // check for overflows
    int result_power = dividend->power - divisor->power + (_compare_enlarged_mantisses(dividend->mantissa, divisor->mantissa) >= 0 ? 1 : 0);
    if (result_power > 99999 || result_power < -99999)
        return OVERFLOW;

    if (big_float_is_one(divisor))
    {
        *output = *dividend;
        return SUCCESS;
    }

    output->mantissa_sign = dividend->mantissa_sign * divisor->mantissa_sign;
    output->power = result_power;
    _divide_mantisses(dividend->mantissa, divisor->mantissa, output->mantissa);

    return SUCCESS;
}

status_code_t parse_big_float(const char *float_str, big_float_t *big_float, short int *pos)
{
    assert(float_str != NULL); // float_str must not be NULL
    assert(big_float != NULL); // big_float must not be NULL

    const char *float_str_start = float_str;

    if (strlen(float_str) == 0)
    {
        if (pos != NULL)
            *pos = 0;
        return EMPTY_INPUT;
    }

    // reset big_float structure
    reset_big_float(big_float);

    // check for positive/negative sign infront
    if (*float_str == '-')
    {
        big_float->mantissa_sign = NEGATIVE;
        float_str++;
    }
    else if (*float_str == '+')
        float_str++;

    unsigned short int curr_digit_index = 0;
    unsigned char curr_digit_pair = 0;

    short int add_power = 0; // additinal power (caused by floating point)

    bool was_any_digit = false;
    bool was_zeros = false;

    // skip leading zeros
    while (*float_str == '0')
    {
        float_str++;
        was_zeros = true;
    }

    // read digits before dot
    while (isdigit((unsigned char)*float_str))
    {
        was_any_digit = true;

        curr_digit_pair = 10 * curr_digit_pair + (*float_str - '0');
        curr_digit_index++;

        if (curr_digit_index > 2 * DIGIT_PACKS_AMOUNT) // limit exceed
        {
            if (pos != NULL)
                *pos = float_str - float_str_start;
            return MANTIS_LIMIT_EXCEED;
        }
        else if (curr_digit_index % 2 == 0) // pair constructed
        {
            big_float->mantissa[curr_digit_index / 2 - 1] = curr_digit_pair;
            curr_digit_pair = 0;
        }

        float_str++;
        add_power++;
    }

    // detect dot
    // if detected - read after dot
    if (*float_str == '.')
    {
        float_str++;

        int ending_zeros_amount = 0;

        // "skip" zeros
        while (*float_str == '0')
        {
            float_str++;
            // add_power--;
            ending_zeros_amount++;
            was_zeros = true;
        }

        while (isdigit((unsigned char)*float_str))
        {
            // subtract zeros amount from power
            add_power -= ending_zeros_amount;
            ending_zeros_amount = 0;

            was_any_digit = true;

            curr_digit_pair = 10 * curr_digit_pair + (*float_str - '0');
            curr_digit_index++;

            if (curr_digit_index > 2 * DIGIT_PACKS_AMOUNT) // limit exceed
            {
                if (pos != NULL)
                    *pos = float_str - float_str_start;
                return MANTIS_LIMIT_EXCEED;
            }
            else if (curr_digit_index % 2 == 0) // pair constructed
            {
                big_float->mantissa[curr_digit_index / 2 - 1] = curr_digit_pair;
                curr_digit_pair = 0;
            }

            float_str++;
        }
    }

    // check if any digit was meeted
    if (!was_any_digit && !was_zeros)
    {
        if (pos != NULL)
            *pos = float_str - float_str_start;
        return UNEXPECTED_SYMBOL;
    }

    // pack unpaired digit
    if (curr_digit_index % 2 == 1)
        big_float->mantissa[curr_digit_index / 2] = 10 * curr_digit_pair;

    // detect exponent
    // if detected - read power
    if (*float_str == 'e' || *float_str == 'E')
    {
        float_str++;

        if (*float_str == '\0')
        {
            if (pos != NULL)
                *pos = float_str - float_str_start;
            return UNEXPECTED_EOF;
        }

        // get power sign
        char power_sign = POSITIVE;
        if (*float_str == '-')
        {
            power_sign = NEGATIVE;
            float_str++;
        }
        else if (*float_str == '+')
            float_str++;

        if (*float_str == '\0')
        {
            if (pos != NULL)
                *pos = float_str - float_str_start;
            return UNEXPECTED_EOF;
        }

        while (isdigit((unsigned char)*float_str))
        {
            if (big_float->power > 99999 || big_float->power < -99999) // limit exceed
            {
                if (pos != NULL)
                    *pos = float_str - float_str_start;
                return POWER_LIMIT_EXCEED;
            }

            big_float->power = 10 * big_float->power + (*float_str - '0');

            if (big_float->power > 99999 || big_float->power < -99999) // limit exceed
            {
                if (pos != NULL)
                    *pos = float_str - float_str_start;
                return POWER_LIMIT_EXCEED;
            }

            float_str++;
        }

        big_float->power *= power_sign;
    }

    // use additinal power
    big_float->power += add_power;
    // if (big_float->power > 99999 || big_float->power < -99999)
    //     return OVERFLOW;

    // detect end of number
    if (*float_str != '\0')
    {
        if (pos != NULL)
            *pos = float_str - float_str_start;
        return UNEXPECTED_SYMBOL;
    }

    return SUCCESS;
}

status_code_t input_big_float(big_float_t *big_float, short int *pos)
{
    assert(big_float != NULL); // big_float must not be NULL

    char float_str[MAX_INPUT_STRING_LENGTH];
    if (fgets(float_str, MAX_INPUT_STRING_LENGTH, stdin) != float_str)
        return CANT_READ_INPUT;

    // remove last endline symbols
    while (float_str[strlen(float_str) - 1] == '\n' || float_str[strlen(float_str) - 1] == '\r')
        float_str[strlen(float_str) - 1] = '\0';

    return parse_big_float(float_str, big_float, pos);
}

status_code_t float_to_big_float(float float_num, big_float_t *big_float, short int *pos)
{
    assert(big_float != NULL); // big_float must not be NULL

    char float_str[MAX_INPUT_STRING_LENGTH];
    char format[10];
    sprintf(format, "%%.%ig", 2 * DIGIT_PACKS_AMOUNT);
    sprintf(float_str, format, float_num);

    return parse_big_float(float_str, big_float, pos);
}

float big_float_to_float(big_float_t *big_float)
{
    float result = 0.0f;

    for (short int i = 0; i < DIGIT_PACKS_AMOUNT; i++)
        result += big_float->mantissa[i] * powf(100.0f, -i - 1);

    result *= big_float->mantissa_sign * powf(10, big_float->power);
    return result;
}

/**
 *  @brief Переводит число в строку.
 * 
 *  Формат вывода: [+-]0.000000000000000000000000000000E[+-]00000
 */
void big_float_to_str(big_float_t *big_float, char *str)
{
    assert(big_float != NULL); // big_float must not be NULL
    assert(str != NULL);       // str must not be NULL

    *str = big_float->mantissa_sign == POSITIVE ? '+' : '-';
    str++;

    *str = '0';
    str++;
    *str = '.';
    str++;

    int digits = _count_digits(big_float);

    for (int i = 0; i < (digits + 1) / 2; i++)
    {
        unsigned char pair = big_float->mantissa[i];
        *str = '0' + pair / 10;
        str++;

        if (2 * i + 1 == digits)
            break;

        *str = '0' + pair % 10;
        str++;
    }

    *str = 'E';
    str++;

    int power = big_float->power;
    if (power < 0)
    {
        *str = '-';
        power *= -1;
    }
    else
    {
        *str = '+';
    }

    str++;
    sprintf(str, "%i", power);
}

/**
 *  @brief Выводит число в стандартный поток вывода.
 */
void output_big_float(big_float_t *big_float)
{
    assert(big_float != NULL); // big_float must not be NULL

    char float_str[1 + 2 * DIGIT_PACKS_AMOUNT + 10];
    big_float_to_str(big_float, float_str);
    fputs(float_str, stdout);
}
