#ifndef __STATUS_CODES_H_
#define __STATUS_CODES_H_

#ifdef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifdef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

enum
{
    STACK_INVALID = 2,
    STACK_EMPTY,
    STACK_OVERFLOW
};

#endif
