#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
int mark;
static int choose(int n)
{
	return(rand()%n);
}
static inline void gen_rand_op(){
	switch(choose(4)){
		case 0:{
	          strcat(buf,"+");
		  break;
		}
		case 1:{
                 strcat(buf,"-");		  
                 break;
		}
		case 2:{
	         strcat(buf,"*");	
		  break;
		}
		case 3:{
	           strcat(buf,"/");	
		  break;
		}
	} 
}
        
static inline void gen_rand_expr() {
   if(mark>10)
   {
      unsigned num1=choose(20);
      char number1[5];
      sprintf(number1,"%d",num1);
      strcat(buf,number1);
      strcat(buf,"u");
   }
   else
   switch(choose(3)){
	  case 0: {
	    unsigned num1=choose(20);
	    char number1[5];
	    sprintf(number1,"%d",num1);
	    strcat(buf,number1);
	    strcat(buf,"u");
	    break;
	  }
	  case 1: {
	    strcat(buf,"(");
	    gen_rand_expr();
	    strcat(buf,")");
	    mark++;
	    break;
          }
	  default: {
            gen_rand_expr();
	    gen_rand_op();
	    gen_rand_expr();
	    break;
	  }
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s;"
"  unsigned mr=0;  "
"  printf(\"%%u \",mr); "
"  printf(\"%%u\",result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  unsigned seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) { 
    mark=0;
    memset(buf,0,sizeof(buf));
    gen_rand_expr();
    sprintf(code_buf, code_format, buf);
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);
    int mark1=1;
    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    unsigned result;
    fscanf(fp, "%d%u",&mark1, &result);
    pclose(fp);
    if(mark1==0)
    {
      printf("%u ", result);
      for(int ii=0;ii<strlen(buf);ii++)
      {
	      if((buf[ii]=='+') || (buf[ii]=='-') || (buf[ii]=='*') || (buf[ii]=='/'))
              {
		  int kk=choose(2);
		  for(int jj=1;jj<=kk;jj++) printf(" ");
		  printf("%c",buf[ii]);
	          kk=choose(2);
                  for(int jj=1;jj<=kk;jj++) printf(" ");
	      }
	      else 
              if(buf[ii]!='u')
              printf("%c",buf[ii]);
      }
      printf("\n");
    }
   }
    return 0;
}
