#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stack.h"

const uint64_t STACK_POISON = 0xDEADBEEF;

stack_status stack_construct(stack_t *st, size_t size)
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
    return stack_validate(st);
}

stack_status stack_destruct(stack_t *st)
{
    if (st == NULL)
        return STACK_OK;
    stack_status stat = stack_validate(st);
    if (stat != STACK_OK)
        return stat;
    free(st->data);
    #ifdef STACK_DEBUG
    memset(st, STACK_POISON, sizeof(*st));
    #endif
    return STACK_OK;
}

stack_t *stack_new(size_t size)
{
    stack_t *st = calloc(1, sizeof(*st));
    if (st == NULL)
        return NULL;
    stack_construct(st, size);
    return st;
}

stack_status stack_delete(stack_t *st)
{
    stack_status status = stack_destruct(st);
    if (status != STACK_OK)
        return status;
    free(st);
    return STACK_OK;
}

stack_status stack_push(stack_t *st, stack_elem_t elem)
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

stack_status stack_pop(stack_t *st)
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
stack_status stack_back(stack_t *st, stack_elem_t *out)
{
    STACK_VALIDATE(st);
    if (st->size == 0)
        return STACK_UNDERFLOW;
    *out = st->data[st->size - 1];
    return STACK_OK;
}

stack_status stack_erase(stack_t *st)
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

bool stack_empty(stack_t *st)
{
    if (st == NULL)
        return true;
    STACK_VALIDATE(st);
    return st->size == 0;
}

size_t stack_getsize(stack_t *st)
{
    if (st == NULL)
        return 0;
    STACK_VALIDATE(st);
    return st->size;
}

void stack_dump(stack_t *st)
{
    if (st == NULL)
    {
        fprintf(stderr, "stack_t [%p] {}\n", st);
        return;
    }
    fprintf(stderr, "stack_t [%p] {\n", st);
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
        fprintf(stderr, "    *[%lu] = %lf aka %#lX,\n", i, elem.ste, elem.ui);
    }
    for (size_t i = st->size; i < st->capacity; ++i)
    {
        elem.ste = st->data[i];
        fprintf(stderr, "     [%lu] = %lf aka %#lX,\n", i, elem.ste, elem.ui);
    }
    fprintf(stderr, "}\n");
}

stack_status stack_validate(stack_t *st)
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