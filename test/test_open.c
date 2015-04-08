/*
 *
 *
 *
 */

/* #include <sys/stat.h> */
/* #include <sys/types.h> */

/* #include <fcntl.h> */

#define O_RDONLY 0x1
extern int open(const char *path, int flags);
extern int close(int fd);

int
main(void)
{
	int fd;

	fd = open("/dev/null", O_RDONLY);
	close(fd);
	return (0);
}
