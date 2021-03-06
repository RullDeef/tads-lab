#ifndef STATUS_CODES_H_
#define STATUS_CODES_H_

typedef enum
{
    SUCCESS,
    FAILURE,
    CANT_READ_INPUT,
    EMPTY_INPUT,
    UNEXPECTED_SYMBOL,
    UNEXPECTED_EOF,
    MANTIS_LIMIT_EXCEED,
    POWER_LIMIT_EXCEED,
    OVERFLOW,

    DIVISION_BY_ZERO
} status_code_t;

#endif
