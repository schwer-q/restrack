#include <string.h>
#include <stdlib.h>


int
main(int argc, char * const argv[])
{
    char *buffer;
    if (argc == 2)
    {
        buffer = (char *)malloc(sizeof(*buffer) * 64);
        buffer = strcat(buffer, argv[1]);
    }
    return (EXIT_SUCCESS);
}
