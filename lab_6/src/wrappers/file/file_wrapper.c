#include "utils/colors.h"
#include "file_wrapper.h"

struct file_wrapper fw_create(const char *name)
{
    struct file_wrapper fw;
    fw.name = name;
    fw.file = fopen(name, "a+");

    fseek(fw.file, 0L, SEEK_END);
    fw.size = ftell(fw.file);

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
    fseek(fw->file, 0L, SEEK_END);
    fw->size = ftell(fw->file);
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
    fprintf(file, "Файл:\n");

    fseek(fw->file, 0L, SEEK_SET);

    int num;
    while (fscanf(fw->file, "%d", &num) == 1)
        fprintf(file, "%d ", num);
    fprintf(file, "\n");

    fprintf(file, "Размер структуры: " CLR_BR_GREEN "%lu" CLR_RESET " байт.\n", fw_calc_size(fw));
    fprintf(file, "Среднее число сравнений в структуре: " CLR_BR_GREEN "%.2f" CLR_RESET ".\n\n", fw_calc_mean_cmp_amount(fw));

    return 0;
}

size_t fw_calc_size(struct file_wrapper *fw)
{
    return fw->size;
}

float fw_calc_mean_cmp_amount(struct file_wrapper *fw)
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
