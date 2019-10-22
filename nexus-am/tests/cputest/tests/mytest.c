#include "trap.h"

int main()
{
    char out[1024];
    nemu_assert(sprintf(out, "\n") == 2);
}