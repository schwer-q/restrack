#include <stdlib.h>

int
main(void)
{
    int *ptr;

    ptr = (int *)malloc(sizeof(*ptr) * 1);
    if (ptr)
    {
        *ptr = 42;
    }
    return (EXIT_SUCCESS);
}
