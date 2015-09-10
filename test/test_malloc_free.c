void *malloc(int sz);
void free(void *p)

void
myalloc(void)
{
  void *ptr;

  ptr = malloc(1024);
  free(ptr);
}
