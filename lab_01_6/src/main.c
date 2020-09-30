#include <stdio.h>
#include "status_codes.h"
#include "big_float/big_float.h"

#ifndef TEST
status_code_t handle_input_errors(status_code_t status_code, short int pos)
{
    if (status_code == EMPTY_INPUT)
    {
        printf("Вы не ввели числа. Перезапустите "
               "программу, и повторите попытку.\n");
    }
    else if (status_code == UNEXPECTED_SYMBOL)
    {
        for (short int i = 0; i < pos; i++)
            printf(" ");
        printf("^\n");

        printf("Неожиданный символ в числе. Пожалуйста, "
               "проверьте соответствие формата ввода и "
               "перезапустите программу.\n");
    }
    else if (status_code == UNEXPECTED_EOF)
    {
        for (short int i = 0; i < pos; i++)
            printf(" ");
        printf("^\n");

        printf("Неожиданный конец ввода. "
               "Пожалуйста, перезапустите программу.\n");
    }
    else if (status_code == MANTIS_LIMIT_EXCEED)
    {
        for (short int i = 0; i < pos; i++)
            printf(" ");
        printf("^\n");

        printf("Суммарная длина мантиссы не должна превышать 30 цифр. "
            "Пожалуйста, перезапустите программу.\n");
    }
    else if (status_code == POWER_LIMIT_EXCEED)
    {
        for (short int i = 0; i < pos; i++)
            printf(" ");
        printf("^\n");

        printf("Суммарная длина степени не должна превышать 5 цифр. "
               "Пожалуйста, перезапустите программу.\n");
    }
    else if (status_code == OVERFLOW)
    {
        printf("Введеное число, будучи приведённым к нормализованному виду, вызывает переполнение. "
            "Пожалуйста, перезапустите программу.\n");
    }

    return status_code;
}
#endif

status_code_t input_numbers(big_float_t *bf_1, big_float_t *bf_2)
{
    status_code_t result;
    short int pos;

#ifndef TEST
    printf("Введите первое число в любом из предложенных форматов:\n"
        "  [+-]000[.000][e[+-]000]\n"
        "  [+-].000[e[+-]000]\n"
        "|1-------|10-------|20-------|30-------\n");
#endif

    result = input_big_float(bf_1, &pos);

#ifndef TEST
    result = handle_input_errors(result, pos);
#endif

    if (result != SUCCESS)
        return result;

#ifndef TEST
    printf("\nТеперь введите второе число\n"
        "|1-------|10-------|20-------|30-------\n");
#endif

    result = input_big_float(bf_2, &pos);

#ifndef TEST
    result = handle_input_errors(result, pos);
#endif

    if (result != SUCCESS)
        return result;

    return result;
}

void output_result(big_float_t *big_float, status_code_t status_code)
{
#ifdef TEST
    if (status_code == SUCCESS)
    {
        output_big_float(big_float);
        printf("\n");
    }
#else
    printf("\n");
    if (status_code == DIVISION_BY_ZERO)
        printf("Деление на ноль не имеет смысла.\n");
    else if (status_code == OVERFLOW)
        printf("При попытке деления произошло переполнение.\n");
    else
    {
        printf("Результат деления в нормализованном виде:\n");
        output_big_float(big_float);
    }
#endif
}

int main(void)
{
    big_float_t bf_1, bf_2, bf_3;
    status_code_t status;

    status = input_numbers(&bf_1, &bf_2);
    if (status != SUCCESS)
        return status;

    status = divide_big_float(&bf_1, &bf_2, &bf_3);
    output_result(&bf_3, status);

    return status;
}