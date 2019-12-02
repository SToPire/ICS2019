#include "common.h"
int printf(const char* fmt, ...);
_Context* schedule(_Context* prev);
_Context* do_syscall(_Context* c);
static _Context* do_event(_Event e, _Context* c)
{
    switch (e.event) {
        case _EVENT_SYSCALL: do_syscall(c); break;
        case _EVENT_YIELD:
            printf("Hey YIELD!\n");
            return schedule(c);
        default: panic("Unhandled event ID = %d", e.event);
    }

    return NULL;
}

void init_irq(void)
{
    Log("Initializing interrupt/exception handler...");
    _cte_init(do_event);
}