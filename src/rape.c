#include <stdio.h>
#define stack_elem_t double
#define ELEM_PRINT "%lf"
#include "stack.h"
#undef stack_elem_t

int main(int argc, char const *argv[])
{
    stack_double *st = stack_new_double(1);
    stack_push_double(st, 2);
    stack_push_double(st, 3);
    st->data[1] = 8;
    stack_push_double(st, 20);
    stack_delete_double(st);
    return 0;
}