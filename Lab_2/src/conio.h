#ifndef __CONIO_H_
#define __CONIO_H_

#include <stdio.h>
#include <string.h>
#include <time.h>

#define CIO_OK              0
#define CIO_FGETS_ERROR     1
#define CIO_OVERFLOW        2
#define CIO_INPUT_EMPTY     3

int cio_read_line(char *str, int max_length);
int cio_read_uchar(unsigned char *num);
int cio_read_ulong(unsigned long *num);
int cio_read_float(float *num);
int cio_read_time(time_t *time);

int cio_parse_int(char **str, int *num);

#endif // __CONIO_H_
