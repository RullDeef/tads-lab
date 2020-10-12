#pragma once

#include <string.h>

typedef struct {
    size_t __alloc_sz;
    char* __data;
    char* top;
} str_stack_t;

extern "C" {
    __declspec(dllexport) str_stack_t strStCreate();
}
