#ifndef __CONIO_H_
#define __CONIO_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "flat.h"

#define CIO_OK              0
#define CIO_FGETS_ERROR     1
#define CIO_OVERFLOW        2
#define CIO_INPUT_EMPTY     3

#define MIN_POSSIBLE_YEAR 1880
#define MAX_POSSIBLE_YEAR 2020

int cio_read_line(char *str, int max_length);
int cio_read_uchar(unsigned char *num);
int cio_read_ulong(unsigned long *num);
int cio_read_float(float *num);
int cio_read_time(time_t *time);

int cio_parse_int(char **str, int *num);
int cio_request_flat_data(flat_t *flat);

void cio_colorize_set(uint32_t num);
void cio_colorize_flush(void);

#endif // __CONIO_H_
