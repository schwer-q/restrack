#include <stdlib.h>

typedef struct {
    int *foo;
    int *bar;
} Test;

void
scopped_malloc(int *ptr)
{
    ptr = (int *)malloc(sizeof(*ptr) * 1);
}

void
test_manipulation(Test *test)
{
    scopped_malloc(test->foo);
    test->bar = (int *)malloc(sizeof(*(test->bar)) * 1);
}

void
test_free(Test *test)
{
    test->bar = NULL;
}

int
main(void)
{
    Test test;

    test_manipulation(&test);
    *(test.foo) = 42;
    *(test.bar) = 24;
    free(test.bar);
    test_free(&test);
    return (EXIT_SUCCESS);
}
