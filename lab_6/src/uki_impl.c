#include "callocs.h"

#define UKI_MALLOC(size) callocs_malloc(size)
#define UKI_FREE(ptr) callocs_free(ptr)
#define UKI_IMPL
#include "uki.h"
