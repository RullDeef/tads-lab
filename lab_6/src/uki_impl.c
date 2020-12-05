#include "callocs.h"

#define UKI_MENU_INFO_SIZE 512U

#define UKI_MALLOC(size) callocs_malloc(size)
#define UKI_FREE(ptr) callocs_free(ptr)
#define UKI_IMPL
#include "uki.h"
