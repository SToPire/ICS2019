#include "trap.h"

int main()
{
    char out[1024];
    //printf("%s", "hello");
    //int a = sprintf(out, "%s", "hello");
    //int a = sprintf(out, "hello");
    //printf("%d\n", a);
    nemu_assert(sprintf(out, "%010s", "hello") == 10);
    nemu_assert(strcmp(out, "00000hello") == 0);
}