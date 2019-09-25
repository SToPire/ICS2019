#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[10000];
static int now=0;

uint32_t choose(uint32_t n){
	return rand()%n;
}

static inline void gen_space(){
	switch(choose(4)){
		case 0:buf[now++]=' ';break;
		default:break;
	}
}

static inline void gen(char c){
	buf[now++]=c;
}
static inline void gen_num(){
	buf[now++]=choose(9)+'1';//avoid 0 prefix
	int len=choose(8)+1;
	int i;
	for(i=1;i<=len;i++){
		uint32_t t=choose(10);
		buf[now++]=t+'0';
	}
	buf[now++]='u'; //unsigned 
}

static inline void gen_rand_op(){
	switch(choose(4)){
		case 0:buf[now++]='+';break;
		case 1:buf[now++]='-';break;
		case 2:buf[now++]='*';break;
		case 3:buf[now++]='/';break;
	}
}

static inline void gen_rand_expr(int i) {
	if(i>=10) return;
  	switch(choose(3)){
		case 0:gen_space();gen_num();gen_space();break;
		case 1:if(i<9){gen_space();gen('(');gen_space();gen_rand_expr(i+1);gen_space();gen(')');gen_space();break;}else{gen_space();gen_num();gen_space();break;}
		case 2:if(i<9){gen_space();gen_rand_expr(i+1);gen_space();gen_rand_op();gen_space();gen_rand_expr(i+1);gen_space();break;}else{gen_space();gen_num();gen_space();break;}
  	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  FILE *fp = fopen(\"/tmp/log\",\"w\");"  //log file, 1 will be written if exception, else 12 will be written
"  fprintf(fp,\"0\");"
"  fclose(fp);"  
                                       	
"  unsigned result = %s; "	
"  fp = fopen(\"/tmp/log\",\"w\");"									
"  fprintf(fp,\"1\");"											
"  fclose(fp);"

"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  printf("%d\n",loop);
  int i;
  for (i = 0; i < loop; i ++) {
	now = 0;
    gen_rand_expr(0);
	buf[now]='\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);
    uint32_t result;
    fscanf(fp, "%u", &result);
	FILE *fp2 = fopen("/tmp/log","r");
	
	int NoERROR;
	fscanf(fp2,"%d",&NoERROR);
	fclose(fp2);

	if(NoERROR){                     //No exception
		pclose(fp);
    	printf("%u %s\n" , result, buf);
	}
	else{                 //exception happened. Run one more iteration.
		--i;
		pclose(fp);
	}
  }
  return 0;
}
