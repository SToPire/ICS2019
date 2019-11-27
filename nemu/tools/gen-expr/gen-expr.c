#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
// this should be enough
static char buf[65536];
static int s[65536];
int t[66356];
int now=0,dep,num=0;
static inline void gen_rand_expr();
static inline void gen_num(){
	int k=rand()%3;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	int type=rand()%2;
	if(type==1)
		buf[now++]='(',buf[now++]='-';
	int x=rand()%20+1,y=x,w[10],len=0;
	if(type==1) y=-x;
	while(x){
		w[++len]=x%10;
		x/=10;
	}
	s[++num]=y;t[num]=1;
	for(int i=len;i;--i)
		buf[now++]=w[i]+'0';
	k=rand()%2;if(type==1) buf[now++]=')';
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
} 
static inline void gen_exp1(){
	int k=rand()%2;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	buf[now++]='(',s[++num]='(';t[num]=0;
	k=rand()%2;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	++dep;gen_rand_expr();--dep;
	k=rand()%2;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	buf[now++]=')';s[++num]=')';t[num]=0;
	k=rand()%2;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
} 
static inline void gen_exp2(){
	int k=rand()%3;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	++dep;gen_rand_expr();--dep;
	k=rand()%3;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	int type=rand()%4;
	switch(type){
		case 0:buf[now++]='+';break;
		case 1:buf[now++]='-';break;
		case 2:buf[now++]='*';break;
		case 3:buf[now++]='/';break;
	}
	s[++num]=buf[now-1];t[num]=0;
	k=rand()%2;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
	++dep;gen_rand_expr();--dep;
	k=rand()%2;
	for(int i=1;i<=k;++i)
		buf[now++]=' ';
} 
static inline void gen_rand_expr() {
	int type=rand()%3,k;
	if(dep>10) gen_num();
	else
	switch(type){
		case 0:gen_num();break;
		case 1:gen_exp1();break;
		case 2:gen_exp2();break;
	}
	buf[now]='\0';
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned int result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
int sta[1001],top1,f[1001],top2;
int table[256][256];
int check(){
	top1=0;f[top2=0]='#';s[num+1]='#';t[num+1]=0;
	for(int i=1;i<=num+1;++i){
		//printf("%d\n",i);
		if(t[i]==1)
			sta[++top1]=s[i];
		else{
			if(top2){
				while(table[f[top2]][s[i]]==2){
						int b=sta[top1--],a=sta[top1--];
						//printf("%d %d %c %c\n",a,b,f[top2-1],f[top2]);
						if(f[top2]=='/'){
							//printf("yes\n"); 
							if(b==0) return 0;
							else a=a/b;
						}
						if(f[top2]=='*'){
							a=a*b;
						}
						if(f[top2]=='-'){
							a=a-b;
						}
						if(f[top2]=='+'){
							a=a+b;
						}
						top2--;sta[++top1]=a;
					}
					
				if(table[f[top2]][s[i]]==1){
					if(s[i]=='#') return 1;
					else top2--;
				}
				else if(table[f[top2]][s[i]]==0) f[++top2]=s[i];
			}
			else f[++top2]=s[i];
		}
	}
		return 1;
}
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  table['+']['+']=2;table['+']['-']=2;table['+']['*']=0;table['+']['/']=0;table['+']['(']=0;table['+'][')']=2;table['+']['#']=2;
  table['-']['+']=2;table['-']['-']=2;table['-']['*']=0;table['-']['/']=0;table['-']['(']=0;table['-'][')']=2;table['-']['#']=2;
  table['*']['+']=2;table['*']['-']=2;table['*']['*']=2;table['*']['/']=2;table['*']['(']=0;table['*'][')']=2;table['*']['#']=2;
  table['/']['+']=2;table['/']['-']=2;table['/']['*']=2;table['/']['/']=2;table['/']['(']=0;table['/'][')']=2;table['/']['#']=2;
  table['(']['+']=0;table['(']['-']=0;table['(']['*']=0;table['(']['/']=0;table['(']['(']=0;table['('][')']=1;table['(']['#']=3;
  table[')']['+']=2;table[')']['-']=2;table[')']['*']=2;table[')']['/']=2;table[')']['(']=3;table[')'][')']=2;table[')']['#']=2;
  table['#']['+']=0;table['#']['-']=0;table['#']['*']=0;table['#']['/']=0;table['#']['(']=0;table['#'][')']=3;table['#']['#']=1;
  //printf("%d\n",loop);
  for (i = 0; i < loop; i ++) {
    now=0;dep=1;num=0;
    gen_rand_expr();
    //return 0;
    while(!check()){
        //printf("yes\n");
    	now=0;dep=1;num=0;
    	gen_rand_expr();
    }
    sprintf(code_buf, code_format, buf);
    FILE *fp = fopen("/tmp/.code.c", "w");
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

    printf("%u %s\n", result, buf);
  }
  return 0;
}
