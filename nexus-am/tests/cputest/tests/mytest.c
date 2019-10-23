#include "trap.h"

int main()
{
    char out[1024];
    //printf("%s", "hello");
    sprintf(out, "%s", "hello");
    //nemu_assert(sprintf(out, "%s", "hello") == 5);
    //nemu_assert(sprintf(out, "hello") == 5);
    for (int i = 0; i <= 5; i++) printf("%d\n", out[i]);
    //nemu_assert(strcmp(out, "hello") == 0);
}