#include <stdlib.h>

int
main(void)
{
    int *ptr;

    ptr = (int *)malloc(sizeof(*ptr) * 42);
    if (ptr)
        free(ptr);
    free(ptr);
    return (EXIT_SUCCESS);
}
