#ifndef __MENU_H_
#define __MENU_H_

#include "utils/colors.h"

#include "wrappers/bst/bst_wrapper.h"
#include "wrappers/avl/avl_wrapper.h"
#include "wrappers/hash_table/ht_wrapper.h"
#include "wrappers/file/file_wrapper.h"

#define PROG_INTRO      "    " CLR_BR_GREEN_U "Лабораторная работа №6 по типам и структурам данных." CLR_RESET "\n\n" \
                        "Список доступных комманд:"

int menu_main(void);

int menu_worker(struct bst_wrapper *bstw, struct avl_wrapper *avlw, struct ht_wrapper *htw, struct file_wrapper *fw);

#endif // __MENU_H_
