#include <common.h>

void main_loop();
void hal_init();

int main(void)
{
    Log("game start!");
    Log("ss");
    hal_init();
    main_loop();

    return 0;
}
