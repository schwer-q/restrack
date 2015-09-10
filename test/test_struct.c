void *malloc(int sz);
void free(void *p);
int open(const char *p, int m);

#define O_RDONLY 1

struct t {
  int fd;
};

void
test()
{
  struct t *p;

  p = malloc(sizeof(struct t));

  p->fd = open("/dev/null", O_RDONLY);
  free(p);
}
