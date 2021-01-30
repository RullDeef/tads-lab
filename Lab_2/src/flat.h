#ifndef __FLAT_H_
#define __FLAT_H_

#include <sys/types.h>
#include <assert.h>

#define FIELDS_DELIMETER ';'
#define PRIMARY_TYPE_STR "PRIMARY"
#define SECONDARY_TYPE_STR "SECONDARY"
#define TRUE_STR "yes"
#define FALSE_STR "no"
#define MAX_ADDRESS_SIZE (30 + 1)

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

typedef struct
{
    char address[MAX_ADDRESS_SIZE];
    float area;
    uint8_t rooms_amount;
    float price_per_m2;
    uint8_t type;
    union
    {
        uint8_t has_trim; // 0 or 1
        uint8_t was_pets;
    } type_data;
    time_t build_time;
    uint8_t prev_owners_amount;
    uint8_t prev_lodgers_amount;
} flat_t;

typedef struct
{
    uint32_t id;
    uint8_t rooms_amount;
} flat_key_t;

flat_t create_flat();
flat_t clone_flat(flat_t *original);

/*
    Expected string format:

    <address>;<area>;<rooms_amount>;<price_per_m2>;<type>;<has_trim|was_pets>[;<build_time>;<prev_owners_amount>;<prev_lodgers_amount>]

    <type> is one of: PRIMARY, SECONDARY
*/
int sread_flat(const char *str, flat_t *flat);
void printf_flat(uint32_t id, flat_t *flat);

void assign_flat(void *flat_1, void *flat_2);
void assign_flat_key(void *flat_1, void *flat_2);


#endif // __FLAT_H_
