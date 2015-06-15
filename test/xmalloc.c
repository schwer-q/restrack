/*
 *
 *
 *
 */

extern void *malloc(int sz);

void *
do_alloc(void)
{
	void *ptr1;

	ptr1 = malloc(100);
	return (ptr1);
}
