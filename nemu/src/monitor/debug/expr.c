#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-" , '-'},			// sub
  {"\\*", '*'},  		// multiply
  {"/", '/'},			// divide
  {"\\(", '(' },		// left bracket
  {"\\)", ')' },		// right bracket
  {"==", TK_EQ},        // equal
  {"0*[1-9][0-9]*",TK_NUM},		// num
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }

  
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
			case TK_NOTYPE: break;
			case '+': tokens[nr_token].type='+';
					  strcpy(tokens[nr_token].str,"+");
					  ++nr_token;
					  break;
			case '-': tokens[nr_token].type='-';
					  strcpy(tokens[nr_token].str,"-");
					  ++nr_token;
					  break;
			case '*': tokens[nr_token].type='*';
					  strcpy(tokens[nr_token].str,"*");
					  ++nr_token;
					  break;
			case '/': tokens[nr_token].type='/';
					  strcpy(tokens[nr_token].str,"/");
					  ++nr_token;
					  break;
			case '(': tokens[nr_token].type='(';
					  strcpy(tokens[nr_token].str,"(");
					  ++nr_token;
					  break;
			case ')': tokens[nr_token].type=')';
					  strcpy(tokens[nr_token].str,")");
					  ++nr_token;
					  break;
			case TK_NUM: tokens[nr_token].type=TK_NUM;
						 while(*substr_start=='0' && substr_len>1){ ++substr_start; --substr_len; } //delete 0 in prefix
						 sprintf(tokens[nr_token].str,"%.*s",substr_len,substr_start);
						 ++nr_token;
						 break;
			default: TODO();
        }

        break;
      }

    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

int check_parentheses(int p, int q){
	int i;
	int cnt=0;
	bool flag=false;
	for(i=p;i<=q;i++){
		if(tokens[i].type=='(') ++cnt;
		else if(tokens[i].type==')') --cnt;
		if(cnt<0) return 0;
		if((cnt==0 && (i!=p && i!=q))) flag=true;
	}
	if(cnt) return 0;
	else if(flag) return -1;
	else if(tokens[p].type=='(' && tokens[q].type==')') return 1;
	return 23333; //will not be executed forever, just for pass the compliation.
}

int find_main_operator(int p,int q){
	bool in_brackets=false;
	int now='*';
	int ans=0;
	int i;
	for(i=p;i<=q;i++){
		if(tokens[i].type=='(') in_brackets=true;
		else if(tokens[i].type==')') in_brackets=false;
		else if((tokens[i].type=='*' || tokens[i].type=='/')
					&& now=='*' && !in_brackets)  ans=i;
		else if((tokens[i].type=='+' || tokens[i].type=='-')
					&& !in_brackets){ ans=i;now='+';}
	}
	return ans;
}

uint32_t eval(int p,int q){
	if(p>q){
		printf("Bad Expression!\n");
		return 0;
	}
	else if(p==q){
		if(tokens[p].type==TK_NUM) return atoi(tokens[p].str);
		else{
			printf("Bad Expression!\n");
			return 0;
		}
	}
	else{
		int v=check_parentheses(p,q);
		if(v==1) return eval(p+1,q-1); 			// match brackets
		else if(v==0){
			printf("Bad Expression!\n");
			return 0;
		}										// illegal expression
		else if(v==-1){
			int op=find_main_operator(p,q);	printf("op=%d\n",op);// legal, but p & q don't match
			uint32_t val1=eval(p,op-1);
			uint32_t val2=eval(op+1,q);
			switch(tokens[op].type){
				case '+':return val1 + val2;
				case '-':return val1 - val2;
				case '*':return val1 * val2;
				case '/':return val1 / val2;
			}
		}
	}
	return 0;
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  printf("%d",eval(0,nr_token-1));
  return 0;
}
