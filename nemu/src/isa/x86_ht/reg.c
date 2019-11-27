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
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  printf("*cpu.eax = %d = 0x%08x\n",cpu.eax,cpu.eax);
  printf("*cpu.ecx = %d = 0x%08x\n",cpu.ecx,cpu.ecx);
  printf("*cpu.edx = %d = 0x%08x\n",cpu.edx,cpu.edx);
  printf("*cpu.ebx = %d = 0x%08x\n",cpu.ebx,cpu.ebx);
  printf("*cpu.esp = %d = 0x%08x\n",cpu.esp,cpu.esp);
  printf("*cpu.ebp = %d = 0x%08x\n",cpu.ebp,cpu.ebp);
  printf("*cpu.esi = %d = 0x%08x\n",cpu.esi,cpu.esi);
  printf("*cpu.edi = %d = 0x%08x\n",cpu.edi,cpu.edi);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
	int len=strlen(s);char s1[10];
  for(int i=1;i<len;++i){
		s1[i]=*(s+i);
		//printf("%c",s[i]);
	}
	//printf("%d\n",len);
	for(int i=1;i<len;++i)
		if(s1[i]>='A'&&s1[i]<='Z')
			s1[i]=s1[i]-'A'+'a';
	for(int i=1;i<len;++i){
		if(s1[i]<'a'||s1[i]>'z'){
			*success=0;return 0;
		}
	}
	while(s1[len-1]==' '&&len>=3) len--;
  if(len==3&&s1[1]=='p'&&s1[2]=='c') return cpu.pc;
	for(int i=0;i<8;++i){
		if(strcmp(s1+1,reg_name(i,1))==0) return reg_b(i); 
		if(strcmp(s1+1,reg_name(i,2))==0) return reg_w(i); 
		if(strcmp(s1+1,reg_name(i,4))==0) return reg_l(i); 
	}
	*success=0;return 0;
}
