/*

    uki - библиотека для рутинных функций по работе с консолью, и псевдоинтерфейсом.

    (C) 2020 - Rull Deef

*/

#ifndef __UKI_H_
#define __UKI_H_

/*
    Общие макроопределения для библиотеки uki.

    (C) 2020 - Rull Deef
*/

#ifndef __UKI_DEFINES_H_
#define __UKI_DEFINES_H_

#define UKI_OK                      0
#define UKI_ERROR                   1
#define UKI_MENU_EXIT               -203

#define UKI_TEMP_STR_BUF_LEN        255
#define UKI_FMT_STR_BUF_LEN         20

#define UKI_TABLE_MAX_ROWS          100
#define UKI_TABLE_MAX_COLS          20
#define UKI_TABLE_TITLE_BUF_LEN     255
#define UKI_TABLE_STR_BUF_LEN       64

#define UKI_MENU_TITLE_BUF_LEN      127
#define UKI_MENU_OPT_NAME_BUF_LEN   127
#define UKI_MENU_MAX_OPTS           10

#define UKI_MAX(a, b) ((a) >= (b) ? (a) : (b))

#endif
/*
    Данный модуль содержит вспомогательные функции.

    (C) 2020 - Rull Deef
*/

#ifndef __UKI_UTILS_H_
#define __UKI_UTILS_H_

#include <sys/types.h>
#include <stdint.h>

void uki__safe_strcpy(char *dst, const char *src, uint32_t buf_size);
void uki__center_str(char *str, uint32_t size);

void uki_clear_console(void);
void uki_wait(const char *msg);

#endif
/*
    Данный модуль содержит функции по обработке ввода с клавиатуры.

    Префикс constaint означает, что ввод будет проверяться на заданные ограничения.
    Префикс force означает, что ввод будет требоваться в цикле, пока не будет введён корректно.

    Все функции возвращающие логическое значение возвращают true, если ввод был корректным.

    (C) 2020 - Rull Deef
*/

#ifndef __UKI_INPUT_H_
#define __UKI_INPUT_H_

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>


typedef bool (*uki_constraint_int32_t)(int32_t);
typedef bool (*uki_constraint_uint32_t)(uint32_t);
typedef bool (*uki_constraint_float_t)(float);
typedef bool (*uki_constraint_double_t)(double);
typedef bool (*uki_constraint_str_t)(const char*);

bool uki_input_int32(const char *msg, const char *err_msg, int32_t *value);
bool uki_input_int32_minmax(const char *msg, const char *err_msg, int32_t min, int32_t max, int32_t *value);
bool uki_input_int32_constraint(const char *msg, const char *err_msg, int32_t *value, uki_constraint_int32_t constraint);
bool uki_input_int32_force(const char *msg, const char *repeat_msg, int32_t *value);
bool uki_input_int32_force_minmax(const char *msg, const char *repeat_msg, int32_t min, int32_t max, int32_t *value);
bool uki_input_int32_force_constraint(const char *msg, const char *repeat_msg, int32_t *value, uki_constraint_int32_t constraint);

bool uki_input_uint32(const char *msg, const char *err_msg, uint32_t *value);
bool uki_input_uint32_minmax(const char *msg, const char *err_msg, uint32_t min, uint32_t max, uint32_t *value);
bool uki_input_uint32_constraint(const char *msg, const char *err_msg, uint32_t *value, uki_constraint_uint32_t constraint);
bool uki_input_uint32_force(const char *msg, const char *repeat_msg, uint32_t *value);
bool uki_input_uint32_force_minmax(const char *msg, const char *repeat_msg, uint32_t min, uint32_t max, uint32_t *value);
bool uki_input_uint32_force_constraint(const char *msg, const char *repeat_msg, uint32_t *value, uki_constraint_uint32_t constraint);

bool uki_input_float(const char *msg, const char *err_msg, float *value);
bool uki_input_float_minmax(const char *msg, const char *err_msg, float min, float max, float *value);
bool uki_input_float_constraint(const char *msg, const char *err_msg, float *value, uki_constraint_float_t constraint);
bool uki_input_float_force(const char *msg, const char *repeat_msg, float *value);
bool uki_input_float_force_minmax(const char *msg, const char *repeat_msg, float min, float max, float *value);
bool uki_input_float_force_constraint(const char *msg, const char *repeat_msg, float *value, uki_constraint_float_t constraint);

bool uki_input_double(const char *msg, const char *err_msg, double *value);
bool uki_input_double_minmax(const char *msg, const char *err_msg, double min, double max, double *value);
bool uki_input_double_constraint(const char *msg, const char *err_msg, double *value, uki_constraint_double_t constraint);
bool uki_input_double_force(const char *msg, const char *repeat_msg, double *value);
bool uki_input_double_force_minmax(const char *msg, const char *repeat_msg, double min, double max, double *value);
bool uki_input_double_force_constraint(const char *msg, const char *repeat_msg, double *value, uki_constraint_double_t constraint);

// Для ввода строки в статический буфер
bool uki_input_str(const char *msg, const char *err_msg, char *value, uint32_t buf_size);
bool uki_input_str_force(const char *msg, const char *repeat_msg, char *value, uint32_t buf_size);
bool uki_input_str_constraint(const char *msg, const char *err_msg, char *value, uint32_t buf_size, uki_constraint_str_t constraint);
bool uki_input_str_force_constraint(const char *msg, const char *repeat_msg, char *value, uint32_t buf_size, uki_constraint_str_t constraint);

// вариант сканфа, но в одну строку
int uki_input_scanf_line(const char *msg, const char *fmt, ...);

#endif
/*
    Данный модуль содержит функции для вывода таблиц.

    (C) 2020 - Rull Deef
*/

#ifndef __UKI_TABLE_H_
#define __UKI_TABLE_H_

#include <sys/types.h>
#include <stdint.h>


typedef struct
{
    uint32_t rows;
    uint32_t cols;
    char title[UKI_TABLE_TITLE_BUF_LEN];
    char data[UKI_TABLE_MAX_ROWS][UKI_TABLE_MAX_COLS][UKI_TABLE_STR_BUF_LEN];
} uki_table_t;

uki_table_t uki_table_create(uint32_t rows, uint32_t cols, const char *title);

// true если записалось удачно
bool uki_table_set(uki_table_t *table, uint32_t row, uint32_t col, const char *str);
bool uki_table_set_fmt(uki_table_t *table, uint32_t row, uint32_t col, const char *fmt, ...);

void uki_table_print(const uki_table_t *table);

#endif
/*
    Данный модуль содержит функции для работы с консольным меню.

    (C) 2020 - Rull Deef
*/

#ifndef __UKI_MENU_H_
#define __UNI_MENU_H_

#include <sys/types.h>


// Функция обработчик пунктов меню
// Должна вернуть UKI_MENU_EXIT чтобы выйти из меню корректно
typedef int (*uki_menu_opt_fn_t)(void *);

typedef struct
{
    char name[UKI_MENU_OPT_NAME_BUF_LEN];
    uki_menu_opt_fn_t func;
} uki_menu_opt_t;

typedef struct
{
    uint32_t opts_amount;
    char title[UKI_MENU_TITLE_BUF_LEN];
    uki_menu_opt_t opts[UKI_MENU_MAX_OPTS];
} uki_menu_t;

uki_menu_t uki_menu_create(const char *title, uint32_t opts_amount, ...);

// Стандартная функция для выхода из меню
int uki_menu_opt_exit(void *data);

// Возвращает UKI_OK, если не было никаких ошибок
int uki_menu_run(const uki_menu_t *menu, void* data);

#endif
#endif
