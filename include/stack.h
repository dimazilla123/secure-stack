
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "bool.h"

#ifndef stack_elem_t
#define stack_elem_t int
#define ELEM_PRINT "%d"
#endif

#define CONCAT(a, b) a ## _ ## b
#define OVERLOAD(name, type) CONCAT(name, type)
#define GENERIC(name) OVERLOAD(name, stack_elem_t)

typedef struct GENERIC(stack_str) GENERIC(stack);

#ifdef STACK_DEBUG
const uint64_t STACK_POISON = 0xDEADBEEF;
#endif

typedef enum
{
    STACK_OK = 0,
    STACK_UNDERFLOW,
    STACK_NULL,
    STACK_NO_MEMORY,
    STACK_NOT_VALID
} stack_status;

struct GENERIC(stack_str)
{
    #ifdef STACK_DEBUG
    size_t checksum;
    #endif
    size_t capacity;
    size_t size;
    stack_elem_t *data;
};

stack_status GENERIC(stack_construct)(GENERIC(stack) *st, size_t size);
stack_status GENERIC(stack_destruct)(GENERIC(stack) *st);
GENERIC(stack)     *GENERIC(stack_new)(size_t size);
stack_status GENERIC(stack_delete)(GENERIC(stack) *st);

stack_status GENERIC(stack_push)(GENERIC(stack) *st, stack_elem_t elem);
stack_status GENERIC(stack_pop)(GENERIC(stack) *st);
stack_status GENERIC(stack_back)(GENERIC(stack) *st, stack_elem_t *out);

stack_status GENERIC(stack_erase)(GENERIC(stack) *st);

bool         GENERIC(stack_empty)(GENERIC(stack) *st);
size_t       GENERIC(stack_getsize)(GENERIC(stack) *st);

stack_status GENERIC(stack_validate)(GENERIC(stack) *st);
void         GENERIC(stack_dump)(GENERIC(stack) *st);

#ifdef STACK_DEBUG
#define STACK_VALIDATE(st) {stack_status stst = GENERIC(stack_validate)(st); if (stst != STACK_OK) return stst;}
#else
#define STACK_VALIDATE(st) {}
#endif

stack_status GENERIC(stack_construct)(GENERIC(stack) *st, size_t size)
{
    if (st == NULL)
        return STACK_NULL;
    #ifdef STACK_DEBUG
    st->checksum = 0;
    #endif
    st->capacity = size; 
    st->size = 0;
    st->data = calloc(size, sizeof(st->data[0]));
    if (st->data == NULL)
        return STACK_NO_MEMORY;
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
    #ifdef STACK_DEBUG
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

stack_status GENERIC(stack_push)(GENERIC(stack) *st, stack_elem_t elem)
{
    STACK_VALIDATE(st);
    if (st->size == st->capacity)
    {
        stack_elem_t *ndata = reallocarray(st->data, st->capacity * 2, sizeof(st->data[0]));
        if (ndata == NULL)
            return STACK_NO_MEMORY;
        st->capacity *= 2;
        st->data = ndata;
        #ifdef STACK_DEBUG
        for (size_t i = st->size; i < st->capacity; ++i)
            *((uint64_t*)&st->data[i]) = STACK_POISON;
        #endif
    }
    st->data[st->size++] = elem;
    STACK_VALIDATE(st);
    return STACK_OK;
}

stack_status GENERIC(stack_pop)(GENERIC(stack) *st)
{
    STACK_VALIDATE(st);
    if (st->size == 0)
        return STACK_UNDERFLOW;
    --st->size;
    #ifdef STACK_DEBUG
    *((uint64_t*)&st->data[st->size]) = STACK_POISON;
    #endif
    return STACK_OK;
}
stack_status GENERIC(stack_back)(GENERIC(stack) *st, stack_elem_t *out)
{
    STACK_VALIDATE(st);
    if (st->size == 0)
        return STACK_UNDERFLOW;
    *out = st->data[st->size - 1];
    return STACK_OK;
}

stack_status GENERIC(stack_erase)(GENERIC(stack) *st)
{
    STACK_VALIDATE(st);
    st->size = 0;
    #ifdef STACK_DEBUG
    for (size_t i = 0; i < st->capacity; ++i)
        *((uint64_t*)&st->data[i]) = STACK_POISON;
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
    if (st == NULL)
    {
        fprintf(stderr, "GENERIC(stack) [%p] {}\n", st);
        return;
    }
    fprintf(stderr, "GENERIC(stack) [%p] {\n", st);
    #ifdef STACK_DEBUG
    fprintf(stderr, "    checksum = %lu,\n", st->checksum);
    #endif
    fprintf(stderr, "    capacity = %lu,\n", st->capacity);
    fprintf(stderr, "    size     = %lu,\n", st->size);
    fprintf(stderr, "    data     = %p,\n",  st->data);
    union {stack_elem_t ste; uint64_t ui;} elem = {};
    for (size_t i = 0; i < st->size; ++i)
    {
        elem.ste = st->data[i];
        fprintf(stderr, "    *[%lu] = " ELEM_PRINT " aka %#lX,\n", i, elem.ste, elem.ui);
    }
    for (size_t i = st->size; i < st->capacity; ++i)
    {
        elem.ste = st->data[i];
        fprintf(stderr, "     [%lu] = " ELEM_PRINT " aka %#lX,\n", i, elem.ste, elem.ui);
    }
    fprintf(stderr, "}\n");
}

stack_status GENERIC(stack_validate)(GENERIC(stack) *st)
#ifdef STACK_DEBUG
{
    stack_dump(st);
    if (st == NULL || st->data == NULL)
        return STACK_NULL;
    if (st->size > st->capacity)
        return STACK_NOT_VALID;
    return STACK_OK;
}
#else
{
    if (st == NULL || st->data == NULL)
        return STACK_NULL;
    if (st->size > st->capacity)
        return STACK_NOT_VALID;
    return STACK_OK;
}
#endif
