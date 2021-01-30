#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "conio.h"

int cio_read_line(char *str, int max_length)
{
    assert(str != NULL);
    assert(max_length > 0);

    char *temp = malloc((max_length + 2) * sizeof(char));
    if (fgets(temp, max_length + 2, stdin) != temp)
        return CIO_FGETS_ERROR;

    if (strlen(temp) == (size_t)max_length + 1)
    {
        free(temp);
        return CIO_OVERFLOW;
    }

    while (strlen(temp) > 0 && (temp[strlen(temp) - 1] == '\n' || temp[strlen(temp) - 1] == '\r'))
        temp[strlen(temp) - 1] = '\0';

    if (strlen(temp) == 0)
    {
        free(temp);
        return CIO_INPUT_EMPTY;
    }

    memcpy(str, temp, strlen(temp) + 1);
    free(temp);
    return CIO_OK;
}

int cio_read_uchar(unsigned char *num)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    unsigned long val = strtoul(temp, &end, 10);

    if (val == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    if (val > 255UL)
        return -4;

    *num = (unsigned char)val;
    return 0;
}

int cio_read_ulong(unsigned long *num)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    *num = strtoul(temp, &end, 10);

    if (*num == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    return 0;
}

int cio_read_float(float *num)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    *num = strtof(temp, &end);

    if (temp == end)
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    return 0;
}

int cio_parse_int(char **str, int *num)
{
    char *end_ptr;

    *num = strtol(*str, &end_ptr, 10);
    if (*num == 0 && (errno == EINVAL || *str == end_ptr))
        return -1;

    *str += (end_ptr - *str);
    return 0;
}

int cio_read_time(time_t *time)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    int days = 0;
    int months = 0;
    int years = 0;

    char *tmp_str = temp;
    if (cio_parse_int(&tmp_str, &days))
    {
        printf("bad number! day\n");
        return -1;
    }

    tmp_str++;
    if (cio_parse_int(&tmp_str, &months))
    {
        printf("bad number! mon\n");
        return -1;
    }

    tmp_str++;
    if (cio_parse_int(&tmp_str, &years))
    {
        printf("bad number! year\n");
        return -1;
    }

    if (days < 1 || 31 < days || months < 1 || 12 < months || years < 1880 || 2020 < years)
    {
        return -2;
    }

    struct tm tmstruct;
    memset((void *)&tmstruct, 0, sizeof(struct tm));

    tmstruct.tm_year = years;
    tmstruct.tm_mon = months - 1;
    tmstruct.tm_mday = days;

    *time = mktime(&tmstruct);

    return 0;
}

static int imp__input_option()
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    while (strlen(temp) > 0 && (temp[strlen(temp) - 1] == '\n' || temp[strlen(temp) - 1] == '\r'))
        temp[strlen(temp) - 1] = '\0';

    if (strcmp(temp, "0") == 0)
        return 0;
    else if (strcmp(temp, "1") == 0)
        return 1;
    else
        return -2;
}

int cio_request_flat_data(flat_t *flat)
{
    assert(flat != NULL);

    int status_code = 0;

    printf("                                               ┌────────────────────────────┐\n"
           "Введите адрес квартиры (не более 30 символов): ");
    status_code = cio_read_line(flat->address, MAX_ADDRESS_SIZE);
    if (status_code == 0)
    {
        printf("Введите площадь квартиры (целое или вещественное положительное число): ");
        status_code = cio_read_float(&flat->area);
    }

    if (status_code == 0)
    {
        printf("Введите количество комнат (натуральное число): ");
        status_code = cio_read_uchar(&flat->rooms_amount);
    }

    if (status_code == 0)
    {
        printf("Введите стоимость за квадратный метр (целое или вещественное положительное число): ");
        status_code = cio_read_float(&flat->price_per_m2);
    }

    if (status_code == 0)
    {
        printf("Введите тип жилья (0 - первичное, 1 - вторичное): ");
        int opt = imp__input_option();
        if (opt < 0)
            status_code = -6;
        else if (opt == 0)
            flat->type = PRIMARY;
        else
            flat->type = SECONDARY;
    }

    if (status_code == 0 && flat->type == PRIMARY)
    {
        printf("Квартира с отделкой? (0 - без отделки, 1 - с отделкой): ");
        int opt = imp__input_option();
        if (opt < 0)
            status_code = -6;
        else
            flat->type_data.has_trim = opt;
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Были ли животные? (0 - нет, 1 - да): ");
        int opt = imp__input_option();
        if (opt < 0)
            status_code = -7;
        else
            flat->type_data.was_pets = opt;
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите дату постройки в формате ДД.ММ.ГГГГ: ");
        status_code = cio_read_time(&flat->build_time);
        if (status_code != 0)
        {
            printf("Вы ввели неверную дату. Переповерьте ввод.\n");
            printf("Дата не должна быть раньше 01.01.1880 или позже 31.12.2020\n");
        }
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите количество предыдущих владельцев (от 1 до 100): ");
        status_code = cio_read_uchar(&flat->prev_owners_amount);
        if (status_code == -4 || flat->prev_owners_amount > 100)
        {
            printf("Количество предыдущих владельцев не соответствует требованиям.\n");
            status_code = -4;
        }
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите количество предыдущих жильцов (от 1 до 100): ");
        status_code = cio_read_uchar(&flat->prev_lodgers_amount);
        if (status_code == -4 || flat->prev_lodgers_amount > 100)
        {
            printf("Количество предыдущих жильцов не соответствует требованиям.\n");
            status_code = -4;
        }
    }

    return status_code;
}

void cio_colorize_set(uint32_t num)
{
    if (num % 5 == 1)
        printf("\033[1;32m");
    else if (num % 5 == 2)
        printf("\033[1;33m");
    else if (num % 5 == 3)
        printf("\033[1;35m");
    else if (num % 5 == 4)
        printf("\033[1;36m");
}

void cio_colorize_flush(void)
{
    printf("\033[0m");
}
