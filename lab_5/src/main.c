#include <stdio.h>
#include "menu/menu.h"

int main(void)
{
    stdin = freopen("in.txt", "rt", stdin);
    return menu_main();
}
