#include "trap.h"

int main()
{
    char out[1024];
    printf("%s", "hello");
    nemu_assert(sprintf(out, "%s", "hello") == 5);
    //nemu_assert(strcmp(out, "hello") == 0);
}