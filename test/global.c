/*
 *
 *
 *
 */

extern void *malloc(int sz);

void *ptr1;

int
do_alloc(void)
{
	ptr1 = malloc(100);
	return (0);
}
