#ifndef __STR_STACK_LIB_H
#define __STR_STACK_LIB_H

#define __API__ __declspec(dllexport)

#include <string.h>

#pragma push pack(4)

struct str_stack_t {
    size_t __alloc_sz;
    size_t __curr_sz;
    char* __data;
    char* top;
};

__API__ struct str_stack_t strStCreate();
__API__ void strStFree(struct str_stack_t* stack);
__API__ void strStResize(struct str_stack_t* stack, size_t size);

__API__ int strStPush(struct str_stack_t* stack, char value);
__API__ int strStPop(struct str_stack_t* stack, char* value);

__API__ int strStRead(struct str_stack_t* stack, char* value);

#endif
