#include <stdint.h> 
#include <stdbool.h>
#include<stdio.h>
#include<string.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *, bool *);
uint32_t ans;
char str[2000];
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  FILE *fp=fopen("/home/yjher/ics2019/nemu/tools/gen-expr/input","r");
  bool ok=1;int t=100;
  //fscanf(fp,"%d",&t);
  while(t--){
  	fgets(str,1024,fp);
    bool okay=1;
    //printf("%s\n",str);
    int j=0;ans=0;int len=strlen(str);
    str[len-1]='\0';len--;
    for(j=0;j<len;++j){
    	if(str[j]==' ')  break;
    	ans=ans*10+str[j]-48;
    }//printf("%s\n",str+j);
   // printf("%u\n",ans);
    j++;
  	uint32_t ans1=expr(str+j,&okay);
  	if(okay==0||(ans1!=ans)){
  		printf("There is something wrong in your calculation of %s\n",str);
  		ok=0;printf("%u %d %s\n",ans,j,str+j);
  		printf("%u %u\n",ans1,ans);
  		//return 0;
  	}
  }
  //return 0;
  if(ok) printf("There is nothing wrong in your calculation of expression!!\n");
  else return 0;
 
  int is_batch_mode =init_monitor(argc, argv);//;
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
