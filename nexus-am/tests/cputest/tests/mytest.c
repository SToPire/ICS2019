#include "trap.h"

int main()
{
    char out[1024];
    nemu_assert(snprintf(out, 1, "hello") == 5);
}