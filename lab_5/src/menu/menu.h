#ifndef __MENU_MAIN_H_
#define __MENU_MAIN_H_

#include "libcmdf.h"
#include "core/worker.h"

#define DOC_HEADER          "Документированные команды:"
#define UNDOC_HEADER        "Недокументированные команды:"

#define CMD_EXIT_HELP       "Выйти из программы."

#define CMD_BACK_HELP       "Перейти в главное меню."


extern worker_params_t wk_params;

int menu_main(void);

int menu_manual(cmdf_arglist *arglist);
int menu_config(cmdf_arglist *arglist);
int menu_run(cmdf_arglist *arglist);

#endif
