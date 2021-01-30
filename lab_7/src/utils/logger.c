#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "logger.h"

#include "colors.h"

#define INFO_COLOR CLR_MAKE(__CLR_CYAN_FG ";" __CLR_BOLD)
#define ERROR_COLOR CLR_MAKE(__CLR_RED_FG ";" __CLR_BOLD)
#define LOG_CLR_RESET CLR_RESET

static FILE *log_file;

int log_init(const char *fname)
{
    int status = 0;

    if (strcmp(fname, "@stdout"))
        log_file = stdout;
    else if (strcmp(fname, "@stderr"))
        log_file = stderr;
    else
    {
        log_file = fopen(fname, "at");
        if (log_file == NULL)
            status = -1;
    }

    return status;
}

void log_destroy(void)
{
    if (log_file != NULL && log_file != stdout && log_file != stderr)
    {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_info(const char *fmt, ...)
{
    if (log_file != NULL)
    {
        fprintf(log_file, INFO_COLOR "[info:] ");

        va_list args;
        va_start(args, fmt);
        vfprintf(log_file, fmt, args);
        va_end(args);

        fprintf(log_file, LOG_CLR_RESET "\n");
    }
}

void log_error(const char *fmt, ...)
{
    if (log_file != NULL)
    {
        fprintf(log_file, ERROR_COLOR "[error:] ");

        va_list args;
        va_start(args, fmt);
        vfprintf(log_file, fmt, args);
        va_end(args);

        fprintf(log_file, LOG_CLR_RESET "\n");
    }
}
