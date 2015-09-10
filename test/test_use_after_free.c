void *malloc(int sz);
void free(void *p);
int write(int fd, void *buf, int sz);  

void
myalloc(void)
{
  void *ptr;

  ptr = malloc(1024);
  free(ptr);
  write(0, ptr, sizeof(ptr));  
}
