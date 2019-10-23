#include "trap.h"

int main()
{
    char out[1024];
    printf("%2s", "hello");
    //int a = sprintf(out, "%s", "hello");
    //int a = sprintf(out, "hello");
    //printf("%d\n", a);
    nemu_assert(sprintf(out, "%2s", "hello") == 5);
    nemu_assert(strcmp(out, "     hello") == 0);
}