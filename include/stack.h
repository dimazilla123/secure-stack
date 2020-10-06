#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"

/* [TODO]
   - Fix premade exploits
   - Add canaries for buffer
   + Lower resize
   ? MOAR RAPE TESTS
  +- MOAR COLORS FOR DUMP
 */

/*!
 * @file General stack realisation
 * Should have predefined stack_elem_t and ELEM_PRINT for printf format\n
 * All functions with name GENERIC(f) should be called as f_stack_elem_t()\n
 * Typenames should be one worded, use typedef for something complicated
 * (i.e. typedef long long ll and stack_push_ll)
 */

#ifndef stack_elem_t
#define stack_elem_t int
#define ELEM_PRINT "%d"
#endif

#ifdef STACK_DEBUG
#define STACK_CANARY_PROTECT
#define STACK_USE_POISON
#define STACK_CHECKSUM_PROTECT
#define STACK_USE_VALIDATE
#endif

#define STR(s)
#define CONCAT(a, b) a ## _ ## b
#define OVERLOAD(name, type) CONCAT(name, type)
#define GENERIC(name) OVERLOAD(name, stack_elem_t)

typedef struct GENERIC(stack_str) GENERIC(stack);

#ifdef STACK_USE_POISON
const uint64_t STACK_POISON = 0xDEADBEEF;
#endif
#ifdef STACK_CANARY_PROTECT
const uint64_t LEFT_CANARY = 0xB14D1;
const uint64_t RIGHT_CANARY = 0xB14D2;
#endif

typedef enum
{
    STACK_OK = 0,
    STACK_UNDERFLOW,
    STACK_NULL,
    STACK_NO_MEMORY,
    STACK_NOT_VALID,
    STACK_WRONG_CHECKSUM,
    STACK_CANARY
} stack_status;

struct GENERIC(stack_str)
{
    #ifdef STACK_CANARY_PROTECT
    unsigned long checksum;
    #endif
    #ifdef STACK_CANARY_PROTECT
    uint64_t left_c;
    #endif
    size_t capacity;
    size_t size;
    stack_elem_t *data;
    #ifdef STACK_CANARY_PROTECT
    uint64_t right_c;
    #endif
};
/*!
 * Returns pointer to stack with starting capacity `size`
 *
 * @param size the initial capacity
 * @return NULL if memory cannot be allocated, valid pointer otherwise
 */
GENERIC(stack) *GENERIC(stack_new)(size_t size);
/*!
 * Constructs stack from allocated memory
 *
 * @param[out] st pointer to memory for structure
 * @param size the inital capacity
 *
 * @return Status of command
 */
stack_status    GENERIC(stack_construct)(GENERIC(stack) *st, size_t size);
/*!
 * Destroys stack without removing structure memory
 */
stack_status    GENERIC(stack_destruct)(GENERIC(stack) *st);
/*!
 * Completely deletes stack from memory
 */
stack_status    GENERIC(stack_delete)(GENERIC(stack) *st);

stack_status    GENERIC(stack_push)(GENERIC(stack) *st, stack_elem_t elem);
stack_status    GENERIC(stack_pop)(GENERIC(stack) *st);
stack_status    GENERIC(stack_back)(GENERIC(stack) *st, stack_elem_t *out);

/*!
 * Deletes all elements from stack
 */
stack_status    GENERIC(stack_erase)(GENERIC(stack) *st);

/*!
 * Checks if stack is empty
 */
bool            GENERIC(stack_empty)(GENERIC(stack) *st);
/*!
 * Returns size of stack. If st == NULL, returns 0
 */
size_t          GENERIC(stack_getsize)(GENERIC(stack) *st);

/*!
 * Checks the validity of stack
 */
stack_status    GENERIC(stack_validate)(GENERIC(stack) *st);
/*!
 * Prints stack contents in fancy way
 */
void            GENERIC(stack_dump)(GENERIC(stack) *st);
/*!
 * Returns string value of error code
 */
const char     *stack_error_code(stack_status status);

#ifdef STACK_USE_VALIDATE
#define STACK_VALIDATE(st) ({\
    stack_status stst = GENERIC(stack_validate)(st);\
    if (stst != STACK_OK)\
    {\
        printf("\033[31mError %s at line %d in %s\033[39m\n", stack_error_code(stst), __LINE__, __func__);\
        return stst;\
    }\
})
#else
#define STACK_VALIDATE(st) {}
#endif

#ifdef STACK_CHECKSUM_PROTECT
unsigned long calc_hash(const unsigned char *data, size_t size)
{
    static const unsigned long radix = 257;
    static const unsigned long mod = 1e9 + 7;
    unsigned long hash = 0;
    for (size_t i = 0; i < size; ++i)
        hash = (radix * hash % mod + data[i]) % mod;
    return hash;
}

void GENERIC(stack_hash)(GENERIC(stack) *st)
{
    st->checksum = 0;
    st->checksum = calc_hash((unsigned char*)st->data, sizeof(st->data[0]) * st->capacity);
    //st->checksum ^= calc_hash((unsigned char*)st, sizeof(st));
}
#endif

stack_status GENERIC(stack_construct)(GENERIC(stack) *st, size_t size)
{
    if (st == NULL)
        return STACK_NULL;
    #ifdef STACK_CHECKSUM_PROTECT
    st->checksum = 0;
    #endif
    #ifdef STACK_CANARY_PROTECT
    st->left_c = LEFT_CANARY;
    st->right_c = RIGHT_CANARY;
    #endif
    st->capacity = size; 
    st->size = 0;
    st->data = calloc(size, sizeof(st->data[0]));
    if (st->data == NULL)
        return STACK_NO_MEMORY;
    #ifdef STACK_CHECKSUM_PROTECT
    GENERIC(stack_hash)(st);
    #endif
    return GENERIC(stack_validate)(st);
}

stack_status GENERIC(stack_destruct)(GENERIC(stack) *st)
{
    if (st == NULL)
        return STACK_OK;
    stack_status stat = GENERIC(stack_validate)(st);
    if (stat != STACK_OK)
        return stat;
    free(st->data);
    #ifdef STACK_USE_POISON
    memset(st, STACK_POISON, sizeof(*st));
    #endif
    return STACK_OK;
}

GENERIC(stack) *GENERIC(stack_new)(size_t size)
{
    GENERIC(stack) *st = calloc(1, sizeof(*st));
    if (st == NULL)
        return NULL;
    GENERIC(stack_construct)(st, size);
    return st;
}

stack_status GENERIC(stack_delete)(GENERIC(stack) *st)
{
    stack_status status = GENERIC(stack_destruct)(st);
    if (status != STACK_OK)
        return status;
    free(st);
    return STACK_OK;
}

stack_status GENERIC(stack_fit)(GENERIC(stack) *st)
{
    if (st->size == st->capacity)
    {
        stack_elem_t *ndata = reallocarray(st->data, st->capacity * 2, sizeof(st->data[0]));
        if (ndata == NULL)
            return STACK_NO_MEMORY;
        st->capacity *= 2;
        st->data = ndata;
        #ifdef STACK_USE_POISON
        for (size_t i = st->size; i < st->capacity; ++i)
            *((uint64_t*)&st->data[i]) = STACK_POISON;
        #endif
    }
    if (st->size * 4 > st->capacity && st->size * 2 < st->capacity)
    {
        stack_elem_t *ndata = reallocarray(st->data, st->capacity / 2, sizeof(st->data[0]));
        if (ndata == NULL)
            return STACK_NO_MEMORY;
        st->capacity /= 2;
        st->data = ndata;
    }
    return STACK_OK;
}

stack_status GENERIC(stack_push)(GENERIC(stack) *st, stack_elem_t elem)
{
    STACK_VALIDATE(st);
    stack_status stat = GENERIC(stack_fit)(st);
    if (stat != STACK_OK)
        return stat;

    st->data[st->size++] = elem;
    #ifdef STACK_CHECKSUM_PROTECT
    GENERIC(stack_hash)(st);
    #endif
    STACK_VALIDATE(st);
    return STACK_OK;
}

stack_status GENERIC(stack_pop)(GENERIC(stack) *st)
{
    STACK_VALIDATE(st);
    if (st->size == 0)
        return STACK_UNDERFLOW;
    --st->size;
    GENERIC(stack_fit)(st);
    #ifdef STACK_POISON
    *((uint64_t*)&st->data[st->size]) = STACK_POISON;
    #endif
    #ifdef STACK_CHECKSUM_PROTECT
    GENERIC(stack_hash)(st);
    #endif
    return STACK_OK;
}

stack_status GENERIC(stack_back)(GENERIC(stack) *st, stack_elem_t *out)
{
    STACK_VALIDATE(st);
    if (st->size == 0)
        return STACK_UNDERFLOW;
    *out = st->data[st->size - 1];
    #ifdef STACK_CHECKSUM_PROTECT
    GENERIC(stack_hash)(st);
    #endif
    return STACK_OK;
}

stack_status GENERIC(stack_erase)(GENERIC(stack) *st)
{
    STACK_VALIDATE(st);
    st->size = 0;
    #ifdef STACK_USE_POISON
    for (size_t i = 0; i < st->capacity; ++i)
        *((uint64_t*)&st->data[i]) = STACK_POISON;
    #endif
    #ifdef STACK_CHECKSUM_PROTECT
    GENERIC(stack_hash)(st);
    #endif
    STACK_VALIDATE(st);
    return STACK_OK;
}

bool GENERIC(stack_empty)(GENERIC(stack) *st)
{
    if (st == NULL)
        return true;
    STACK_VALIDATE(st);
    return st->size == 0;
}

size_t GENERIC(stack_getsize)(GENERIC(stack) *st)
{
    if (st == NULL)
        return 0;
    STACK_VALIDATE(st);
    return st->size;
}

void GENERIC(stack_dump)(GENERIC(stack) *st)
{
    fprintf(stderr, "\033[32m");
    if (st == NULL)
    {
        fprintf(stderr, "GENERIC(stack) [%p] {}\033[39m\n", st);
        return;
    }
    fprintf(stderr, "GENERIC(stack) [%p] {\n", st);
    #ifdef STACK_CHECKSUM_PROTECT
    fprintf(stderr, "    \033[32mchecksum = \033[33m%lu\033[32m,\n", st->checksum);
    #endif
    #ifdef STACK_CANARY_PROTECT
    fprintf(stderr, "    \033[32mleft_c   = \033[33m%#lX\033[32m\n", st->left_c);
    #endif
    fprintf(stderr, "    \033[32mcapacity = \033[33m%lu\033[32m,\n", st->capacity);
    fprintf(stderr, "    \033[32msize     = \033[33m%lu\033[32m,\n", st->size);
    fprintf(stderr, "    \033[32mdata     = \033[33m%p\033[32m,\n",  st->data);
    #ifdef STACK_CANARY_PROTECT
    fprintf(stderr, "    \033[32mright_c  = \033[33m%#lX\033[32m\n", st->right_c);
    #endif
    union {stack_elem_t ste; uint64_t ui;} elem = {};
    for (size_t i = 0; i < st->size; ++i)
    {
        elem.ste = st->data[i];
        fprintf(stderr, "    *[%lu]     = \033[33m" ELEM_PRINT " aka %#lX\033[32m,\n", i, elem.ste, elem.ui);
    }
    for (size_t i = st->size; i < st->capacity; ++i)
    {
        elem.ste = st->data[i];
        fprintf(stderr, "     [%lu]     = \033[33m" ELEM_PRINT " aka %#lX\033[32m,\n", i, elem.ste, elem.ui);
    }
    fprintf(stderr, "}\033[39m\n");
}

stack_status GENERIC(stack_validate)(GENERIC(stack) *st)
{
    GENERIC(stack_dump)(st);
    if (st == NULL || st->data == NULL)
        return STACK_NULL;
    if (st->size > st->capacity)
        return STACK_NOT_VALID;
    #ifdef STACK_CANARY_PROTECT
    if (st->left_c != LEFT_CANARY || st->right_c != RIGHT_CANARY)
        return STACK_CANARY;
    #endif
    #ifdef STACK_CHECKSUM_PROTECT
    unsigned long orig_hash = st->checksum;
    GENERIC(stack_hash)(st);
    if (st->checksum != orig_hash)
        return STACK_WRONG_CHECKSUM;
    #endif
    return STACK_OK;
}

const char *stack_error_code(stack_status status)
{
    static const char *errors[] = 
    {
        "STACK_OK",
        "STACK_UNDERFLOW",
        "STACK_NULL",
        "STACK_NO_MEMORY",
        "STACK_NOT_VALID",
        "STACK_WRONG_CHECKSUM",
        "STACK_CANARY"
    };
    return errors[status];
}

#undef STR
#undef CONCAT
#undef OVERLOAD
#undef GENERIC