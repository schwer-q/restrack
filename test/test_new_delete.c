void *malloc(int sz);
void free(void *p);

void *
new(void)
{
  void *p;

  p = malloc(4096);
  return (p);
}

void
delete(void *ptr)
{
  free(ptr);
}

void
bof(void)
{
  void *ptr;

  ptr = new();
  delete(ptr);
}
