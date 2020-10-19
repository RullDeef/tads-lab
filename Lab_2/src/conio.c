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
