void *malloc(int sz);

void *
new(void)
{
  return (malloc(4096));
}

void
bof(void)
{
  void *ptr;

  ptr = new();
}
