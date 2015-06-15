#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define READ_LIMIT 255

int
main(void)
{
    int fd;
    char buffer[READ_LIMIT] = {0};

    fd = open("open.c", O_RDONLY);
    while (read(fd, buffer, READ_LIMIT) > 0)
        printf("%s\n", buffer);
    return (EXIT_SUCCESS);
}
