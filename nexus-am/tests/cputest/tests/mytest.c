#include "trap.h"

int main()
{
    char out[1024];
    printf("%2d", 1);
    //int a = sprintf(out, "%s", "hello");
    //int a = sprintf(out, "hello");
    //printf("%d\n", a);
    nemu_assert(sprintf(out, "%10d", 12345) == 10);
    //nemu_assert(strcmp(out, "0000012345") == 0);
}