#include "trap.h"

int main()
{
    char out[1024];

    nemu_assert(sprintf(out, "%s", "hello") == 4);
    //nemu_assert(strcmp(out, "hello") == 0);
}