#ifndef __LOGGER_H_
#define __LOGGER_H_

int log_init(const char *fname);
void log_destroy(void);

void log_info(const char *fmt, ...);
void log_error(const char *fmt, ...);

#endif // __LOGGER_H_
