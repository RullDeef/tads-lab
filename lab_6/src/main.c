#include <stdio.h>
#include "utils/logger.h"
#include "callocs.h"
#include "uki.h"
#include "menu/menu.h"

int main(void)
{
    if (log_init("@stdout") != 0)
    {
        fprintf(stderr, "Не удалось инициализировать logger\n");
        return -1;
    }

    int status = 0;

    if (callocs_init(200U * 1024U))
    {
        log_error("Не удалось инициализировать пул памяти");
        status = -2;
    }
    else
    {
        status = uki_init();

        if (status != 0)
        {
            log_error("Не удалось инициализировать ядро");
            status = -3;
        }
        else
        {
            status = menu_main();
            uki_destroy();
        }

        log_info("Пул памяти на момент окончания программы:");
        callocs_fprintf_heap_info(stdout);
        callocs_destroy();
    }

    log_destroy();
    return status;
}
