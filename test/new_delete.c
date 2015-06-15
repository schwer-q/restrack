/*
 *
 *
 *
 */

void *new(void)
{
	return (malloc(1024));
}

void delete(void *p)
{
	free(p);
}

int
main(void)
{
	void *p = new();
	delete(p);
}
