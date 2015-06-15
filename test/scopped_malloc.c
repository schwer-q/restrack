#include <stdlib.h>

int *
scopped_malloc(void)
{
    return ((int *)malloc(sizeof(int) * 1));
}

int
main(void)
{
    int *ptr;

    ptr = scopped_malloc();
    *ptr = 42;
    return (EXIT_SUCCESS);
}
