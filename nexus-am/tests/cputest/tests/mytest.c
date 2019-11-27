#include "trap.h"

int main()
{
    char out[1024];
    // printf("%d-%d-%d %02d:%02d:%02d GMT (", 0, 0, 0, 0, 0, 0);
    printf("%d", 0);
    nemu_assert(sprintf(out, "%4d", -12345) == 6);
    nemu_assert(strcmp(out, "-12345") == 0);
}