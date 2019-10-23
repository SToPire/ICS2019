#include "trap.h"

int main()
{
    char out[1024];
    //printf("%s", "hello");
    //int a = sprintf(out, "%s", "hello");
    //int a = sprintf(out, "hello");
    //printf("%d\n", a);
    nemu_assert(sprintf(out, "%2s", "hello") == 2);
    //nemu_assert(sprintf(out, "hello") == 5);
    //nemu_assert(strcmp(out, "hello") == 0);
}