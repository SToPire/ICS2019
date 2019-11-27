#include<string.h>
#include<stdio.h>
#include"nemu.h"
#include<stdlib.h>
#include<math.h>
#include<sys/types.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e,bool *success);
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
    char c1[65535];
    uint32_t ans;
    bool success;
    FILE *fp=fopen("./tools/gen-expr/input","r");
    assert(fp!=NULL);
    success=true;
    while(fscanf(fp,"%d",&ans)==1)
    {
      fscanf(fp,"%[^\n]",c1);
      uint32_t result1=expr(c1,&success); 
      if(result1==ans) printf("GOOD\n");
      else printf("%u %u %sBAD\n",ans,result1,c1);
    }
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
