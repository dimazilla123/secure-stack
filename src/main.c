#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

/*
Условие:
    Реализуйте свой стек. Решения, использующие std::stack, получат 1 балл.
    Решения, хранящие стек в массиве, получат 1.5 балла.
    Решения, использующие указатели, получат 2 балла.

    Гарантируется, что количество элементов в стеке ни в какой момент времени не превышает 10000.

    Обработайте следующие запросы:
    * push n: добавить число n в конец стека и вывести «ok»;
    * pop: удалить из стека последний элемент и вывести его значение,
      либо вывести «error», если стек был пуст;
    * back: сообщить значение последнего элемента стека, либо вывести «error», если стек пуст;
    * size: вывести количество элементов в стеке;
    * clear: опустошить стек и вывести «ok»;
    * exit: вывести «bye» и завершить работу. 
 */

int main()
{
    stack_t st = {};
    stack_construct(&st, 1);
    char command[7] = {};
    scanf("%6s", command);
    stack_status code = STACK_OK;
    stack_elem_t x = 0;
    while (code == STACK_OK && strcmp("exit", command) != 0)
    {
        #ifdef LOCAL
        fprintf(stderr, "%s\n", command);
        #endif
        if (strcmp(command, "push") == 0)
        {
            scanf("%lf", &x);
            code = stack_push(&st, x);
            printf("ok\n");
        }
        else if (strcmp(command, "pop") == 0)
        {
            code = stack_back(&st, &x);
            code = stack_pop(&st);
            if (code == STACK_UNDERFLOW)
                printf("error\n");
            else printf("%fl\n", x);
            code = STACK_OK;
        }
        else if (strcmp(command, "back") == 0)
        {
            code = stack_back(&st, &x);
            if (code == STACK_UNDERFLOW)
                printf("error\n");
            else printf("%lf\n", x);
            code = STACK_OK;
        }
        else if (strcmp(command, "size") == 0)
        {
            printf("%lu\n", stack_getsize(&st));
        }
        else if (strcmp(command, "clear") == 0)
        {
            code = stack_erase(&st);
            printf("ok\n");
        }
        memset(command, 0, sizeof(command));
        scanf("%6s", command);
        stack_dump(&st);
    }
    printf("bye\n");
    code = stack_destruct(&st);
    return 0;
}