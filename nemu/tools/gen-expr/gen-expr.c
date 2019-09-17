#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static int now=0;

uint32_t choose(uint32_t n){
	return rand()%n;
}

static inline void gen(char c){
	buf[now++]=c;
}
static inline void gen_num(){
	uint32_t t=choose(10);
	buf[now++]=t+'0';
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
		case 0:gen_num();break;
		case 1:if(i<9){gen('(');gen_rand_expr(i+1);gen(')');break;}else{gen_num();break;}
		case 2:if(i<9){gen_rand_expr(i+1);gen_rand_op();gen_rand_expr(i+1);break;}else{gen_num();break;}
  	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
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
  int i;
  for (i = 0; i < loop; i ++) {
	now = 0;
    gen_rand_expr(0);
	buf[now]='\0';
	printf("buf:%s",buf);
    sprintf(code_buf, code_format, buf);
	

    /*FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);*/
  }
  return 0;
}
