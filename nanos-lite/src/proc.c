#include "proc.h"

#define MAX_NR_PROC 4
void naive_uload(PCB* pcb, const char* filename);
void context_kload(PCB* pcb, void* entry);
void context_uload(PCB* pcb, const char* filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB* current = NULL;
PCB* fg_pcb = &pcb[1];
int cnt;
void switch_boot_pcb()
{
    current = &pcb_boot;
}

void hello_fun(void* arg)
{
    int j = 1;
    while (1) {
        Log("Hello World from Nanos-lite for the %dth time!", j);
        j++;
        _yield();
    }
}

void init_proc()
{
    //naive_uload(NULL, "/bin/init");
    //context_kload(&pcb[0], (void*)hello_fun);
    context_uload(&pcb[0], "/bin/hello");
    context_uload(&pcb[1], "/bin/init");
    context_uload(&pcb[2], "/bin/init");
    context_uload(&pcb[3], "/bin/init");

    switch_boot_pcb();
    Log("Initializing processes...");
}
int cnt = 0;
_Context* schedule(_Context* prev)
{
    current->cp = prev;
    //current = &pcb[1];
    //current = (current == &pcb[0] ? fg_pcb : &pcb[0]);
    if (current == &pcb[0]) current = fg_pcb;
    else if(cnt!=10){
        ++cnt;
        current = fg_pcb;
    } else {
        cnt = 0;
        current = &pcb[0];
    }
    return current->cp;
}

void load_for_execve(const char* filename)
{
    naive_uload(current, filename);
}

void change_fg(int key)
{
    fg_pcb = &pcb[key - 1];
}