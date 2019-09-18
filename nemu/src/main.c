/*int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  // Initialize the monitor. 
  int is_batch_mode = init_monitor(argc, argv);

  // Receive commands from user. 
  ui_mainloop(is_batch_mode);

  return 0;
}*/


/* This part of code is made for test */

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<assert.h>
uint32_t expr(char *e, bool *success);
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  // Initialize the monitor. 
  int is_batch_mode = init_monitor(argc, argv);
  
									FILE * fp = fopen("./tools/gen-expr/input","r");
									assert(fp!=NULL);
									int i;
									bool correct = 1;
									uint32_t res1,res2;
									char ex[1024];
									for(i=1;i<=100;i++){			
										fscanf(fp,"%u",&res1);
										fgets(ex,1024,fp);
										int j=0;
										for(j=0;j<=1023;j++){
												if(ex[j]=='\n')ex[j]='\0';
										}
										bool* tmp=0;
													
										FILE * fp2 = freopen("/tmp/nemu-log","w",stdout);
										expr(ex,tmp);
										fclose(fp2);
										
										fp2 = fopen("/tmp/nemu-log","r");
										fscanf(fp2,"%u",&res2);
										fclose(fp2);
										
										if(res1!=res2){
											fprintf(stderr,"Wrong result occured:Line %d Correct answer:%u Wrong answer:%u\n",i,res1,res2);
											correct=0;
										}
									}
									fclose(fp);
									freopen("/dev/tty","w",stdout);
									if(correct) printf("Congratulations!It seems that nothing wrong in your evaluation.\n");

  // Receive commands from user. 
  ui_mainloop(is_batch_mode);

  return 0;
}


