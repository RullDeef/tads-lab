#ifndef __FILE_WRAPPER_H_
#define __FILE_WRAPPER_H_

#include <stdio.h>

struct file_wrapper
{
    const char *name;
    FILE *file;
};

struct file_wrapper fw_create(const char *name);
void fw_destroy(struct file_wrapper *fw);

int fw_insert(struct file_wrapper *fw, int data);

// 0 - найдено, -1 - не найдено
int fw_find(struct file_wrapper *fw, int data);

int fw_fprintf(FILE *file, struct file_wrapper *fw);

size_t fw_calc_size(struct file_wrapper *fw);

#endif // __FILE_WRAPPER_H_
