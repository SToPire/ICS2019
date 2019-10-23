#include "trap.h"

int main()
{
    char out[1024];
    printf("%9d", -12345);
    //int a = sprintf(out, "%s", "hello");
    //int a = sprintf(out, "hello");
    //printf("%d\n", a);
    nemu_assert(sprintf(out, "%04d", -12345) == 6);
    nemu_assert(strcmp(out, "-12345") == 0);
}