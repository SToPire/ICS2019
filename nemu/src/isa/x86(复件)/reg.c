#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  int i;
  for(i=R_EAX;i<=R_EDI;i++)
  {
    printf("%s %d\n",regsl[i],reg_l(i));
  }
  printf("CF %d\n",cpu.EFLAGS.CF);
  printf("OF %d\n",cpu.EFLAGS.OF);
  printf("SF %d\n",cpu.EFLAGS.SF);
  printf("ZF %d\n",cpu.EFLAGS.ZF);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  int bj=0;
  uint32_t re;
  for(int i=R_EAX;i<=R_EDI;i++)
  {  
    if(strcmp(regsl[i],s)==0)
    {
      bj=1;
      re=reg_l(i);
    }
    else
    if(strcmp(regsw[i],s)==0)
    {
      bj=1;
      re=reg_w(i);
    }
    else
    if(strcmp(regsb[i],s)==0)
    {
      bj=1;
      re=reg_b(i);
    }
  }
  if(bj==0) 
  {
    *success=false;
    return 1;
  }
  else return re;
}
