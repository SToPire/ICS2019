int init_monitor(int, char* []);
void ui_mainloop(int);

int main(int argc, char* argv[])
{
    // Initialize the monitor.
    int is_batch_mode = init_monitor(argc, argv);
    //is_batch_mode = 0;
    // Receive commands from user.
    ui_mainloop(is_batch_mode);

    return 0;
}

/* Code above is original main.c */
/* ---------------------------------------------------------------------*/
/* Code below is made for test */

/*#include<stdio.h>
#include<assert.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
uint32_t expr(char *e, bool *success);
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  // Initialize the monitor.
  int is_batch_mode = init_monitor(argc, argv);

  FILE * fp = fopen("./tools/gen-expr/input","r");
  assert(fp!=NULL);

  int i,LOOP;
  bool correct = 1;
  uint32_t res1,res2;
  char ex[1024];
  fscanf(fp,"%d",&LOOP); //read LOOP times
  for(i=1;i<=100;i++){
    fscanf(fp,"%u",&res1);//read correct answer
    fgets(ex,1024,fp); //read expression
    ex[strlen(ex)-1]='\0';
    bool* tmp=0;

    FILE * fp2 = freopen("/tmp/nemu-log","w",stdout);
    printf("%u\n",expr(ex,tmp)); //result is saved in nemu-log file
    fclose(fp2);

    fp2 = fopen("/tmp/nemu-log","r"); //reread result from log file
    fscanf(fp2,"%u",&res2);
    fclose(fp2);

    if(res1!=res2){
      fprintf(stderr,"Wrong result occured:Line %d Correct answer:%u Wrong
answer:%u\n",i,res1,res2); correct=0;
    }
  }
  fclose(fp);
  freopen("/dev/tty","w",stdout); // return the status of stdout
  if(correct) printf("\n\nCongratulations!It seems that nothing wrong in your
evaluation.\n\n");

  // Receive commands from user.
  ui_mainloop(is_batch_mode);

  return 0;
}*/
