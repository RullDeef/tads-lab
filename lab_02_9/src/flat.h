#ifndef __FLAT_H_
#define __FLAT_H_

#include <sys/types.h>
#include <assert.h>

#define FIELDS_DELIMETER ';'
#define PRIMARY_TYPE_STR "PRIMARY"
#define SECONDARY_TYPE_STR "SECONDARY"
#define TRUE_STR "true"
#define FALSE_STR "false"

#ifndef NDEBUG
#define assert_flat(flat)                \
    {                                    \
        assert((flat) != NULL);          \
        assert((flat)->address != NULL); \
        /*assert((flat)->area > 0.0f);*/ \
    }
#else
#define assert_flat(X)
#endif

typedef enum
{
    PRIMARY,
    SECONDARY
} flat_type_t;

typedef unsigned char bit_t;

typedef struct
{
    char *address;
    float area;
    unsigned char rooms_amount;
    float price_per_m2;
    bit_t type;
    union
    {
        bit_t has_trim; // 0 or 1
        bit_t was_pets;
    } type_data;
    time_t build_time;
    unsigned char prev_owners_amount;
    unsigned char prev_lodgers_amount;
} flat_t;

flat_t create_flat();
flat_t clone_flat(flat_t *original);

/*
    Expected string format:

    <address>;<area>;<rooms_amount>;<price_per_m2>;<type>;<has_trim|was_pets>[;<build_time>;<prev_owners_amount>;<prev_lodgers_amount>]

    <type> is one of: PRIMARY, SECONDARY
*/
int sread_flat(const char *str, flat_t *flat);
void printf_flat(flat_t *flat);
void free_flat(flat_t *flat);

#endif // __FLAT_H_
