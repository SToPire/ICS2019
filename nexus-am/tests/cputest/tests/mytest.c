#include "trap.h"

int main()
{
    char out[1024];
    nemu_assert(sprintf(out, "%5s", "hello") == 5);
    nemu_assert(strcmp(out, "hello") == 0);
}