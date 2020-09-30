#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdint.h>
#include "bool.h"

typedef double stack_elem_t;
typedef struct stack_str stack_t;

#ifdef STACK_DEBUG
extern const uint64_t STACK_POISON;
#endif

typedef enum
{
    STACK_OK = 0,
    STACK_UNDERFLOW,
    STACK_NULL,
    STACK_NO_MEMORY,
    STACK_NOT_VALID
} stack_status;

struct stack_str
{
    #ifdef STACK_DEBUG
    size_t checksum;
    #endif
    size_t capacity;
    size_t size;
    stack_elem_t *data;
};

stack_status stack_construct(stack_t *st, size_t size);
stack_status stack_destruct(stack_t *st);
stack_t     *stack_new(size_t size);
stack_status stack_delete(stack_t *st);

stack_status stack_push(stack_t *st, stack_elem_t elem);
stack_status stack_pop(stack_t *st);
stack_status stack_back(stack_t *st, stack_elem_t *out);

stack_status stack_erase(stack_t *st);

bool         stack_empty(stack_t *st);
size_t       stack_getsize(stack_t *st);

stack_status stack_validate(stack_t *st);
void         stack_dump(stack_t *st);

#ifdef STACK_DEBUG
#define STACK_VALIDATE(st) {stack_status stst = stack_validate(st); if (stst != STACK_OK) return stst;}
#else
#define STACK_VALIDATE(st) {}
#endif

#endif