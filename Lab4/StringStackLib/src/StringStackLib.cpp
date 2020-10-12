
#include "StringStackLib.hpp"

#include <stdio.h>

extern "C" {
    void str_stack_t strStCreate()
    {
        printf("created!\n");
        return (str_stack_t) { .__alloc_sz = 0, .__data = NULL, .top = NULL };
    }
}
