/*
 *
 *
 *
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>

int
main(void)
{
	int fd;

	fd = open("/dev/null", O_RDONLY);
	close(fd);
	return (0);
}
