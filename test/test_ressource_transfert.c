void *malloc(int sz);
void free(void *p);

void
bof(void)
{
  void *p0, *p1;

  p0 = malloc(100);
  p1 = p0;
  free(p0);
}
