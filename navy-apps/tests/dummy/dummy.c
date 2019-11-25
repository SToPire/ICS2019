#include <stdint.h>
#include<stdio.h>
#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main()
{
     printf("Passsssssssssssssssss!\n");
   // return _syscall_(SYS_yield, 0, 0, 0);
    return 0;
}
