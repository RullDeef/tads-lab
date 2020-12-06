#include "file_wrapper.h"

struct file_wrapper fw_create(const char *name)
{
    struct file_wrapper fw;
    fw.name = name;
    fw.file = fopen(name, "a+");
    return fw;
}

void fw_destroy(struct file_wrapper *fw)
{
    if (fw->file != NULL)
    {
        fclose(fw->file);
        fw->file = NULL;
    }
}

int fw_insert(struct file_wrapper *fw, int data)
{
    fseek(fw->file, 0L, SEEK_END);
    fprintf(fw->file, " %d", data);
    return 0;
}

// 0 - найдено, -1 - не найдено
int fw_find(struct file_wrapper *fw, int data)
{
    fseek(fw->file, 0L, SEEK_SET);

    int num;
    while (fscanf(fw->file, "%d", &num) == 1 && num != data)
        ;
    
    return num == data ? 0 : -1;
}

int fw_fprintf(FILE *file, struct file_wrapper *fw)
{
    fseek(fw->file, 0L, SEEK_SET);

    int num;
    while (fscanf(fw->file, "%d", &num) == 1)
        fprintf(file, "%d ", num);
    fprintf(file, "\n");

    return 0;
}

size_t fw_calc_size(struct file_wrapper *fw)
{
    (void)fw;
    return sizeof(FILE);
}

float fw_calc_mean_cmp_amont(struct file_wrapper *fw)
{
    int keys = 0;
    int cmps = 0;
    int num;

    rewind(fw->file);
    while (fscanf(fw->file, "%d", &num) != EOF)
    {
        keys++;
        cmps += keys;
    }

    return keys == 0 ? 0.0f : (float)cmps / keys;
}
